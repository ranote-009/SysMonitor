#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <string>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace boost::asio;
using ip::tcp;
namespace pt = boost::property_tree;
// Function to execute a shell command and capture its output
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
    return result;
}



int main() {
  
    try {
        io_service service;
        tcp::socket socket(service);

        tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 12345);
        socket.connect(endpoint);

        pt::ptree systemInfoTree;
        systemInfoTree.put("hostname", exec("hostname"));
        systemInfoTree.put("cpu_usage", exec("top -n 1 | grep 'Cpu(s)'"));
        systemInfoTree.put("ram_usage", exec("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
        systemInfoTree.put("model_name", exec("lscpu | grep 'Model name'"));

        ostringstream systemInfoStream;
        pt::write_json(systemInfoStream, systemInfoTree);
        string systemInfo = systemInfoStream.str();

        // Send the JSON string to the server
        socket.write_some(buffer(systemInfo));

        socket.close();
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}

