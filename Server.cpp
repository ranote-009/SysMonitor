#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/bind/bind.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;
using ip::tcp;
namespace pt = boost::property_tree;

class Acceptor {
private:
    tcp::acceptor acceptor_;
public:
    Acceptor(io_service& service) : acceptor_(service, tcp::endpoint(tcp::v4(), 12345)) {}

    void AcceptConnections() {
        cout << "Server started and waiting for connections..." << endl;
        while (true) {
            tcp::socket socket(acceptor_.get_io_service());
            acceptor_.accept(socket);

            // Start a new thread to handle the client
            thread(HandleClient, std::move(socket)).detach();
        }
    }


};

void HandleClient(tcp::socket socket) {
    try {
        cout << "Client connected: " << socket.remote_endpoint() << endl;

        // Receive data from the client
        char data[1024];
        size_t len = socket.read_some(buffer(data));
        string received_data(data, len);

        // Parse the received JSON data
        pt::ptree received_tree;
        istringstream received_stream(received_data);
        pt::read_json(received_stream, received_tree);

        // Extract and process individual system information fields
        string hostname = received_tree.get<string>("hostname");
        string cpuUsage = received_tree.get<string>("cpu_usage");
        string ramUsage = received_tree.get<string>("ram_usage");
        string modelName = received_tree.get<string>("model_name");

        cout << "Received CPU Usage: " << cpuUsage << endl;
        cout << "Received RAM Usage: " << ramUsage << endl;
        cout << "Received Model Name: " << modelName << endl;
        cout << "Client disconnected: " << socket.remote_endpoint() << endl;
        socket.close();
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
}

int main() {
    try {
        io_service service;
        Acceptor acceptor(service);
        acceptor.AcceptConnections();
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
