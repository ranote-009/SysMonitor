// SystemInfoClient.cpp
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp> 
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "SystemInfoClient.h"

using namespace std;

SystemInfoClient::SystemInfoClient(const std::string& serverAddress, int serverPort, const std::string& logFilePath, const std::string& connectionKey)
    : serverAddress_(serverAddress), serverPort_(serverPort), logFilePath_(logFilePath), reconnectAttempts_(0), connectionKey_(connectionKey), ioc_(), ws_(ioc_, ctx)
{
    ctx.load_verify_file("../../Certificate/server.crt");
    ctx.set_verify_mode(ssl::verify_peer);
}

void SystemInfoClient::run()
{
    try
    {
        connect();
        sendKey();
        while (true)
        {
            sendSystemInfo();
            receiveResponse();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        if (reconnectAttempts_ < 2)
        {
            reconnectAttempts_++;
            ioc_.restart();
            run(); // Reconnect and continue
        }
        else
        {
            std::cout << "Not able to connect" << std::endl;
        }
    }
}

     void SystemInfoClient::sendKey() {
        boost::system::error_code write_error;
        size_t bytes_written = ws_.write(asio::buffer(connectionKey_));

        if (write_error) {
            cerr << "Error sending connection key to the server: " << write_error.message() << endl;
        } else {
            cout << "Sent connection key to the server \n" << endl;
        }
    }

    std::string  SystemInfoClient::exec(const char* cmd) {
        char buffer[128];
        std::string result = "";

        FILE* pipe = popen(cmd, "r");
        if (!pipe) {
            return "Error";
        }
        while (!feof(pipe)) {
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
        }
        pclose(pipe);
        size_t start = result.find_first_not_of(" \t\n\r\f\v");
        size_t end = result.find_last_not_of(" \t\n\r\f\v");

        if (start == std::string::npos || end == std::string::npos) {
            result.clear();
        } else {
            result = result.substr(start, end - start + 1);
        }
        return result;
    }

    void  SystemInfoClient::connect() {
        try {
            asio::ip::tcp::resolver resolver(ioc_);
            auto results = resolver.resolve(serverAddress_, std::to_string(serverPort_));

            asio::connect(ws_.next_layer().next_layer(), results.begin(), results.end());

            beast::error_code ec;
            ws_.next_layer().handshake(ssl::stream_base::client, ec);
            if (ec) {
                throw std::runtime_error("WebSocket handshake failed");
            }

            ws_.handshake(serverAddress_, "/", ec);
            if (ec) {
                throw std::runtime_error("WebSocket handshake failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Connect Exception: " << e.what() << std::endl;
            throw;
        }
    }

    void SystemInfoClient::sendSystemInfo() {
        try {
            pt::ptree systemInfoTree;
            systemInfoTree.put("macaddress", exec("ifconfig | grep -o -E '([0-9a-fA-F]{2}:){5}([0-9a-fA-F]{2})' | paste -d ' ' - -"));
            systemInfoTree.put("hostname", exec("hostname"));
            systemInfoTree.put("cpu_usage", exec("top -b -n 1 | grep '%Cpu(s)' | awk '{print $2+$4+$6+$10+$12+$14}'"));
            systemInfoTree.put("ram_usage", exec("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
            systemInfoTree.put("hdd_utilization", exec("df -h / | tail -1 | awk '{print $5}'"));

            std::ostringstream systemInfoStream;
            pt::write_json(systemInfoStream, systemInfoTree);
            std::string systemInfo = systemInfoStream.str();

            beast::error_code ec;
            size_t bytes_written=ws_.write(asio::buffer(systemInfo), ec);

              if (ec) {
                std::cerr << "Error sending data to the server: " << ec.message() << std::endl;
             } 
             else {
             std::cout << "Sent " << bytes_written << " bytes of data to the server" << std::endl;
              }
            if (ec) {
                throw std::runtime_error("WebSocket write error");
            }
        } catch (const std::exception& e) {
            std::cerr << "Send System Info Exception: " << e.what() << std::endl;
            throw;
        }
    }

 void SystemInfoClient::receiveResponse() {
    string received_data;
         boost::system::error_code read_error;
         bool data_received = false;
        std::array<char, 128> response_data; 
        size_t response_length ;
       // size_t response_length = socket.read_some(buffer(response_data), read_error);

 std::future<size_t> result = std::async(std::launch::async, [&](){
      //  boost::system::error_code read_error;
         beast::flat_buffer buffer;
         beast::error_code read_ec;
         response_length = ws_.read(buffer, read_ec);
       // Adjust the buffer size as needed
     // response_length = ws_.read_some(boost::asio::buffer(response_data), read_error);
        received_data=beast::buffers_to_string(buffer.data());
        if(!read_error) {
            return response_length;
        } else {
            return static_cast<size_t>(0);
        }
    });

    // Creating a timer for 5 seconds
    std::chrono::milliseconds timeout(5000);
    std::future_status status = result.wait_for(timeout);

  

    if (status == std::future_status::ready) {
        size_t response_length = result.get();
        if(response_length > 0) {
            data_received = true;
        }
    }else {
        std::cout << "Data sending failed." << std::endl;
          logSuccess("Data sending failed at, "); // Log success with timestamp
    }

    if (data_received) {
        std::cout <<"Received "<<response_length<< " bytes of Data successfully." << std::endl;
        std::cout <<received_data <<"\n\n"<<std::endl;
          logSuccess(received_data); // Log success with timestamp
    }
    }
    

    void SystemInfoClient::logSuccess(std::string result) {
        std::ofstream logFile(logFilePath_, std::ios::app);
   if (logFile.is_open()) {
     if (logFile.tellp() == 0) {
        // Add column names if the file is empty
        logFile << "Result,Timestamp" << std::endl;
    }
    time_t now = time(0);
    tm* timeInfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeInfo);

    logFile << result << timestamp << std::endl; // Separate values with commas
    logFile.close();
} else {
    std::cerr << "Error opening log file for writing." << std::endl;
}
    
    }
