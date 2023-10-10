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
int i=0;
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
    size_t start = result.find_first_not_of(" \t\n\r\f\v");

    // Find the position of the last non-space character.
    size_t end = result.find_last_not_of(" \t\n\r\f\v");

    // Check for cases where the string is all spaces.
    if (start == std::string::npos || end == std::string::npos) {
        // The string is either empty or contains only spaces.
        result.clear();
    } else {
        // Extract the substring without leading and trailing spaces.
        result= result.substr(start, end - start + 1);
    }
    return result;
}


int main() {
    try {
        io_service service;
        tcp::socket socket(service);

        tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 3000);

        // Connect to the server
        socket.connect(endpoint);
        cout << "Connected to the server" << endl;

        while (1) {
            cout << "Sending info" << endl;
            pt::ptree systemInfoTree;
            systemInfoTree.put("hostname", exec("hostname"));
            systemInfoTree.put("cpu_usage", exec("top -n 1 | grep 'Cpu(s)'"));
            systemInfoTree.put("ram_usage", exec("free -m | awk '/Mem:/ {print $3\" MB used / \"$2\" MB total\"}'"));
            systemInfoTree.put("system_idle_window", exec("top -bn1 | grep Cpu | awk '{printf \"System Idle Window: %.2f%%\", $8}'"));
            

            ostringstream systemInfoStream;
            pt::write_json(systemInfoStream, systemInfoTree);
            string systemInfo = systemInfoStream.str();

            // Send the JSON string to the server
            boost::system::error_code write_error;
           size_t bytes_written = socket.write_some(buffer(systemInfo));//boost::asio::write_some(socket, buffer(systemInfo), write_error);
                   
            if (write_error) {
                cerr << "Error sending data to the server: " << write_error.message() << endl;
                break;  // Exit the loop on write error
            } else {
                cout << "Sent " << bytes_written << " bytes of data to the server" << endl;
            }
            char response_data[1024];
            boost::system::error_code read_error;
            size_t response_length = socket.read_some(buffer(response_data), read_error);

            if (read_error == boost::asio::error::eof) {
                cout << "Server disconnected" << endl;
                break;  // Exit the loop if the server closed the connection
            } else if (read_error) {
                cerr << "Error reading response from the server: " << read_error.message() << endl;
                break;  // Exit the loop on read error
            } else {
                cout << "Received response from the server: " << string(response_data, response_length) << endl;
            }

            // Sleep before the next iteration
            sleep(5);
        }

        // Close the socket when done
        socket.close();
        cout << "Disconnected from the server" << endl;
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        sleep(5);
        if(i<2){
         i++;
         main();
        }else{
             cout<<"Not able to connect"<<endl;
        
        }
       
    }

    return 0;
}
