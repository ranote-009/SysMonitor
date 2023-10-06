#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

using namespace std;
using namespace sql;
using namespace boost::asio;

using ip::tcp;
namespace pt = boost::property_tree;


void HandleClient(tcp::socket&& socket) {
    try {
        cout << "Client connected: " << socket.remote_endpoint() << endl;

      while (1) {
            // Receive data from the client
            char data[1024];
            boost::system::error_code error;
           // size_t len = boost::asio::read(socket, buffer(data), transfer_all(), error);
            size_t len = socket.read_some(buffer(data));

            // if (error == boost::asio::error::eof) {
            //     cout << "Client disconnected: " << socket.remote_endpoint() << endl;
            //     break;  // Exit the loop if the client closed the connection
            // } else if (error) {
            //     throw boost::system::system_error(error);
            // }

            string received_data(data, len);
              cout << "Received " << len << " bytes of data from the client" << endl;
            // Parse the received JSON data
            pt::ptree received_tree;
            istringstream received_stream(received_data);
            pt::read_json(received_stream, received_tree);

            // Extract and process individual system information fields
            string hostname = received_tree.get<string>("hostname");
            string cpuUsage = received_tree.get<string>("cpu_usage");
            string ramUsage = received_tree.get<string>("ram_usage");
            string modelName = received_tree.get<string>("model_name");

            cout << "Received hostname: " << hostname << endl;
            cout << "Received CPU Usage: " << cpuUsage << endl;
            cout << "Received RAM Usage: " << ramUsage << endl;
            cout << "Received Model Name: " << modelName << endl;

            // Initialize MySQL Connector/C++
            sql::mysql::MySQL_Driver *driver;
            driver = mysql::get_mysql_driver_instance();

            // Establish a connection to the MySQL database
            Connection *con;
            con = driver->connect("tcp://localhost:3306", "root", "");
            con->setSchema("Clients_Info");

            // Create a prepared statement
            PreparedStatement *stmt;

            // Define an SQL query to insert data into a table
            string query = "INSERT INTO system_info_table (cpuUsage, ramUsage, hostname) VALUES (?, ?, ?)";

            // Prepare the statement
            stmt = con->prepareStatement(query);

            // Bind the values to the prepared statement
            stmt->setString(1, hostname);
            stmt->setString(2, ramUsage);
            stmt->setString(3, hostname);

            // Execute the SQL query
            stmt->execute();

            // Clean up and close the database connection
            delete stmt;
            delete con;

            // Send a success response to the client
            string successResponse = "Data transfer was successful!";
          //  boost::asio::write(socket, buffer(successResponse));
           socket.write_some(buffer(successResponse));
           cout << "Sent success response to the client" << endl;
        
      }
        // Close the socket when done
        socket.close();
        cout << "Client disconnected: " << socket.remote_endpoint() << endl;
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
}

int main() {
    try {
        io_service service;
        tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 3000));

        cout << "Server started and waiting for connections..." << endl;
        while (1) {
            tcp::socket socket(service);

            // Accept a new connection for each client
            acceptor.accept(socket);

            // Handle the client's request using move semantics
            HandleClient(std::move(socket));
        }
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
