#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace boost::asio;
using ip::tcp;
namespace pt = boost::property_tree;

class SystemInfoClient {
public:
    SystemInfoClient(const std::string& serverAddress, int serverPort, const std::string& logFilePath)
        : serverAddress_(serverAddress), serverPort_(serverPort), logFilePath_(logFilePath) {
    }

    void run() {
        try {
            io_service service;
            tcp::socket socket(service);

            tcp::endpoint endpoint(ip::address::from_string(serverAddress_), serverPort_);

            // Connect to the server
            socket.connect(endpoint);
            cout << "Connected to the server" << endl;

            // Ask the user to enter the login key
            cout << "Enter the login key: ";
            string loginKey;
            cin >> loginKey;

            if (authenticate(loginKey)) {
                cout << "Authentication successful." << endl;
                while (true) {
                    sendSystemInfo(socket);
                    receiveResponse(socket);
                    logSuccess(); // Log success with timestamp
                    sleep(5);
                }
            } else {
                cout << "Authentication failed. Connection terminated." << endl;
            }

            // Close the socket when done
            socket.close();
            cout << "Disconnected from the server" << endl;
        } catch (const std::exception& e) {
            cerr << "Exception: " << e.what() << endl;
            sleep(5);
            if (reconnectAttempts_ < 2) {
                reconnectAttempts_++;
                run();  // Reconnect and continue
            } else {
                cout << "Not able to connect" << endl;
            }
        }
    }

private:
    std::string serverAddress_;
    int serverPort_;
    std::string logFilePath_;
    int reconnectAttempts_ = 0;

    bool authenticate(const std::string& loginKey) {
        // Get the secret key from the environment variable
        const char* envSecretKey = std::getenv("CLIENT_SECRET_KEY");
        if (envSecretKey == nullptr) {
            cerr << "Secret key environment variable not set." << endl;
            return false;
        }

        // Compare the entered loginKey with the secret key from the environment variable
        string secretKey = envSecretKey;
        return loginKey == secretKey;
    }

    string exec(const char* cmd) {
        char buffer[128];
        string result = "";

        FILE* pipe = popen(cmd, "r");
        if (!pipe) {
            return "Error";
        }
        while (!feof(pipe)) {
            if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
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

    void sendSystemInfo(tcp::socket& socket) {
        pt::ptree systemInfoTree;
        systemInfoTree.put("macaddress", exec("ifconfig | grep -o -E '([0-9a-fA-F]{2}:){5}([0-9a-fA-F]{2})' | paste -d ' ' - -"));
        systemInfoTree.put("hostname", exec("hostname"));
        systemInfoTree.put("cpu_usage", exec("top -b -n 1 | grep '%Cpu(s)' | awk '{print $2+$4+$6+$10+$12+$14}'"));
        systemInfoTree.put("ram_usage", exec("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
        systemInfoTree.put("model_name", exec("lscpu | grep 'Model name'"));

        ostringstream systemInfoStream;
        pt::write_json(systemInfoStream, systemInfoTree);
        string systemInfo = systemInfoStream.str();

        boost::system::error_code write_error;
        size_t bytes_written = socket.write_some(buffer(systemInfo));

        if (write_error) {
            cerr << "Error sending data to the server: " << write_error.message() << endl;
        } else {
            cout << "Sent " << bytes_written << " bytes of data to the server" << endl;
        }
    }

    void receiveResponse(tcp::socket& socket) {
        time_t now = time(0);
        tm* timeInfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeInfo);
        char response_data[1024];
        boost::system::error_code read_error;
        size_t response_length = socket.read_some(buffer(response_data), read_error);

        if (read_error == boost::asio::error::eof) {
            cout << "Server disconnected" << endl;
        } else if (read_error) {
            cerr << "Error reading response from the server: " << read_error.message() << endl;
        } else {
            cout << "Received response from the server: " << string(response_data, response_length) << " at " << timestamp << endl;
        }
    }

    void logSuccess() {
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

            logFile << "Data successfully sent at, " << timestamp << std::endl; // Separate values with commas
            logFile.close();
        } else {
            std::cerr << "Error opening log file for writing." << std::endl;
        }
    }
};

int main() {
    SystemInfoClient client("127.0.0.1", 3000, "log.csv");
    client.run();

    return 0;
}
