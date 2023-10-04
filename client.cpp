#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <thread>

using namespace std;
using namespace boost::asio;
using ip::tcp;
namespace pt = boost::property_tree;

class SystemInfoExecutor {
public:
    static string execute(const char* cmd) {
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
        return result;
    }
};

class SystemInfoConnector {
public:
    static void connect(tcp::socket& socket, const tcp::endpoint& endpoint) {
        socket.connect(endpoint);
    }
};

class SystemInfoSender {
public:
    static void send(tcp::socket& socket, const string& message) {
        socket.write_some(buffer(message));
    }
};

class Client {
public:
    Client(const string& serverAddress, int serverPort) : serverAddress_(serverAddress), serverPort_(serverPort) {
        io_service_.reset(new io_service());
        socket_.reset(new tcp::socket(*io_service_));
    }

    void run() {
        try {
            tcp::endpoint endpoint(ip::address::from_string(serverAddress_), serverPort_);
            while (true) {
                SystemInfoConnector::connect(*socket_, endpoint);
                cout << "Sending info" << endl;
                sendSystemInfo();
                cout << "Info sent" << endl;
                sleep(5);
                socket_->close();
            }
        } catch (const std::exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }

private:
    unique_ptr<io_service> io_service_;
    unique_ptr<tcp::socket> socket_;
    string serverAddress_;
    int serverPort_;

    void sendSystemInfo() {
        pt::ptree systemInfoTree;
        systemInfoTree.put("hostname", SystemInfoExecutor::execute("hostname"));
        systemInfoTree.put("cpu_usage", SystemInfoExecutor::execute("top -n 1 | grep 'Cpu(s)'"));
        systemInfoTree.put("ram_usage", SystemInfoExecutor::execute("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
        systemInfoTree.put("model_name", SystemInfoExecutor::execute("lscpu | grep 'Model name'"));

        ostringstream systemInfoStream;
        pt::write_json(systemInfoStream, systemInfoTree);
        string systemInfo = systemInfoStream.str();

        // Send the JSON string to the server
        SystemInfoSender::send(*socket_, systemInfo);
    }
};

int main() {
    Client client("127.0.0.1", 12345);
    client.run();

    return 0;
}

