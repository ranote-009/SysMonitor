#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
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

void HandleClient(tcp::socket socket) {
try{
    

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
        
        cout << "Received hostname: " << hostname<< endl;
        cout << "Received CPU Usage: " << cpuUsage << endl;
        cout << "Received RAM Usage: " << ramUsage << endl;
        cout << "Received Model Name: " << modelName << endl;
         cout << "Client disconnected: " << socket.remote_endpoint() << endl;

            // Initialize MySQL Connector/C++
       // mysql::mysql_driver *driver;
        sql::mysql::MySQL_Driver *driver;
        driver = mysql::get_mysql_driver_instance();

        // Establish a connection to the MySQL database
        Connection *con;
        con= driver->connect("tcp://localhost:3306", "root", "");
        //con = driver->connect("unix_socket=/var/run/mysqld/mysqld.sock", "", "");
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

        // Clean up and close the connection
        delete stmt;
        delete con;
       socket.close();
        
      

}
catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

}


int main() {
    try {
        io_service service;
        tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 12345));
        tcp::socket socket(service);


        cout << "Server started and waiting for connections..." << endl;
        while (1) {
           
            acceptor.accept(socket);

            // Start a new thread to handle the client
            thread(HandleClient, std::move(socket)).detach();
        }
       
       
        
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
  

    return 0;
}

