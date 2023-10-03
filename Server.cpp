#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;
using ip::tcp;
namespace pt = boost::property_tree;

class ClientHandler {
public:
    ClientHandler(tcp::socket socket) : socket_(std::move(socket)) {}

    void Handle() {
        try {
            cout << "Client connected: " << socket_.remote_endpoint() << endl;

            // Receive data from the client
            char data[1024];
            size_t len = socket_.read_some(buffer(data));
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
            cout << "Client disconnected: " << socket_.remote_endpoint() << endl;
            socket_.close();
        } catch (const exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }

private:
    tcp::socket socket_;
};

class Acceptor {
public:
    Acceptor(io_service& service) : acceptor_(service, tcp::endpoint(tcp::v4(), 12345)) {}

    void AcceptConnections() {
        cout << "Server started and waiting for connections..." << endl;
        while (true) {
            tcp::socket socket(acceptor_.get_executor());
            acceptor_.accept(socket);

            // Create a ClientHandler object and handle the client in a new thread
            thread(&ClientHandler::Handle, ClientHandler(std::move(socket))).detach();
        }
    }

private:
    tcp::acceptor acceptor_;
};

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
