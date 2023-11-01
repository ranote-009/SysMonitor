#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp> 
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <thread>


namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ssl = asio::ssl; // SSL namespace
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace pt = boost::property_tree;
using namespace std;

class SystemInfoClient {
public:
    SystemInfoClient(const std::string& serverAddress, int serverPort, const std::string& logFilePath,const std::string& connectionKey)
        : serverAddress_(serverAddress), serverPort_(serverPort), logFilePath_(logFilePath), ioc_(), ws_(ioc_, ctx),connectionKey_(connectionKey) {
        // Load root certificate (you should replace with your CA certificate)
       
        ctx.load_verify_file("server.crt");

        ctx.set_verify_mode(ssl::verify_peer); // Replace with your CA certificate path
    }

    void run() {
        try {
            connect();
            sendKey();
            while (true) {
                sendSystemInfo();
                receiveResponse();
               
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if (reconnectAttempts_ < 2) {
                reconnectAttempts_++;
                ioc_.restart();
                run();  // Reconnect and continue
            } else {
                std::cout << "Not able to connect" << std::endl;
            }
        }
    }

private:
    std::string serverAddress_;
    int serverPort_;
    std::string logFilePath_;
    int reconnectAttempts_ = 0;
    std::string connectionKey_;

    asio::io_context ioc_;
    ssl::context ctx{ssl::context::tlsv12_client};
   websocket::stream<beast::ssl_stream<asio::ip::tcp::socket>> ws_;
    beast::flat_buffer buffer_;

     void sendKey() {
        boost::system::error_code write_error;
        size_t bytes_written = ws_.write(asio::buffer(connectionKey_));

        if (write_error) {
            cerr << "Error sending connection key to the server: " << write_error.message() << endl;
        } else {
            cout << "Sent connection key to the server" << endl;
        }
    }

    std::string exec(const char* cmd) {
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

    void connect() {
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

    void sendSystemInfo() {
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

    void receiveResponse() {
         boost::system::error_code read_error;
         bool data_received = false;
        std::array<char, 128> response_data; 
        size_t response_length ;
       // size_t response_length = socket.read_some(buffer(response_data), read_error);

 std::future<size_t> result = std::async(std::launch::async, [&](){
        boost::system::error_code read_error;
       // Adjust the buffer size as needed
      response_length = ws_.read_some(boost::asio::buffer(response_data), read_error);
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
          logSuccess("Data successfully sent at, "); // Log success with timestamp
    }
    }
    

    void logSuccess(std::string result) {
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
};

int main() {
     boost::property_tree::ptree pt;
    boost::property_tree::read_json("server_info.json", pt);

    // Extract IP address and port
    std::string ipAddress = pt.get<std::string>("ip_address");
    int port = pt.get<int>("port");
    SystemInfoClient client(ipAddress, port, "log.csv","CsGo@2023");
    client.run();

    return 0;
}
