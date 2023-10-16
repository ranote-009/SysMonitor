#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace pt = boost::property_tree;

class SystemInfoClient {
public:
    SystemInfoClient(const std::string& serverAddress, int serverPort, const std::string& logFilePath)
        : serverAddress_(serverAddress), serverPort_(serverPort), logFilePath_(logFilePath), ioc_(), ws_(ioc_) {
    }

    void run() {
        try {
            connect();
            while (true) {
                sendSystemInfo();
                receiveResponse();
                logSuccess(); // Log success with timestamp
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if (reconnectAttempts_ < 2) {
                reconnectAttempts_++;
                ioc_.reset();
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

    asio::io_context ioc_;
    websocket::stream<asio::ip::tcp::socket> ws_;
    beast::flat_buffer buffer_;

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
        asio::ip::tcp::resolver resolver(ioc_);
        auto results = resolver.resolve(serverAddress_, std::to_string(serverPort_));

        asio::connect(ws_.next_layer(), results.begin(), results.end());
        ws_.handshake(serverAddress_, "/");
    }

    void sendSystemInfo() {
        pt::ptree systemInfoTree;
        systemInfoTree.put("macaddress", exec("ifconfig | grep -o -E '([0-9a-fA-F]{2}:){5}([0-9a-fA-F]{2})' | paste -d ' ' - -"));
        systemInfoTree.put("hostname", exec("hostname"));
        systemInfoTree.put("cpu_usage", exec("top -b -n 1 | grep '%Cpu(s)' | awk '{print $2+$4+$6+$10+$12+$14}'"));
        systemInfoTree.put("ram_usage", exec("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
        systemInfoTree.put("model_name", exec("lscpu | grep 'Model name'"));

        std::ostringstream systemInfoStream;
        pt::write_json(systemInfoStream, systemInfoTree);
        std::string systemInfo = systemInfoStream.str();

        ws_.write(asio::buffer(systemInfo));
        ws_.read(buffer_);
    }

    void receiveResponse() {
        ws_.read(buffer_);
        std::cout << "Received response from the server: " << beast::buffers_to_string(buffer_.cdata()) << std::endl;
    }

    void logSuccess() {
        std::ofstream logFile(logFilePath_, std::ios::app);
        if (logFile.is_open()) {
            time_t now = std::time(0);
            tm* timeInfo = localtime(&now);
            char timestamp[20];
            std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeInfo);

            logFile << "Data successfully sent at " << timestamp << std::endl;
            logFile.close();
        } else {
            std::cerr << "Error opening log file for writing." << std::endl;
        }
    }
};

int main() {
    SystemInfoClient client("127.0.0.1", 3000, "log.txt");
    client.run();

    return 0;
}
