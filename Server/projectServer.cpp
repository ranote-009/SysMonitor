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
void CreateTables(Connection* con) {
    try {
        Statement* stmt = con->createStatement();

        // Create the Clients Table if it doesn't exist
        const char* createClientsTableSQL = R"(
            CREATE TABLE IF NOT EXISTS Clients (
                client_id INT AUTO_INCREMENT PRIMARY KEY,
                macaddress VARCHAR(255) NOT NULL
            )
        )";

        // Create the System Info Table if it doesn't exist
        const char* createSystemInfoTableSQL = R"(
            CREATE TABLE IF NOT EXISTS systems_info (
                system_info_id INT AUTO_INCREMENT PRIMARY KEY,
                client_id INT,
                hostname VARCHAR(255) NOT NULL,
                cpuUsage VARCHAR(255) NOT NULL,
                ramUsage VARCHAR(255) NOT NULL,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (client_id) REFERENCES clients(client_id)
            )
        )";

        // Execute the SQL queries to create the tables
        stmt->execute(createClientsTableSQL);
        stmt->execute(createSystemInfoTableSQL);

        delete stmt;
    } catch (const std::exception& e) {
        std::cerr << "Exception while creating tables: " << e.what() << std::endl;
    }
}

void StoreClientInfoInDatabase(Connection* con, const std::string& macaddress) {
    try {
        // Check if the client's hostname already exists in the clients table
        PreparedStatement* checkClientStmt;
        const char* checkClientSQL = "SELECT client_id FROM Clients WHERE macaddress = ?";
        checkClientStmt = con->prepareStatement(checkClientSQL);
        checkClientStmt->setString(1, macaddress);
        ResultSet* resultSet = checkClientStmt->executeQuery();

        int clientId = -1; // Initialize to a value that indicates the hostname is not in the table
        if (resultSet->next()) {
            clientId = resultSet->getInt("client_id");
        }

        // If the client's hostname doesn't exist, insert it into the clients table
        if (clientId == -1) {
            PreparedStatement* insertClientStmt;
            const char* insertClientSQL = "INSERT INTO Clients (macaddress) VALUES (?)";
            insertClientStmt = con->prepareStatement(insertClientSQL);
            insertClientStmt->setString(1, macaddress);
            insertClientStmt->execute();

            // Retrieve the generated client_id
            clientId = insertClientStmt->getUpdateCount();

            delete insertClientStmt;
        }

        delete checkClientStmt;
    } catch (const std::exception& e) {
        std::cerr << "Exception while storing client info: " << e.what() << std::endl;
    }
}

void StoreSystemInfoInDatabase(const std::string& macaddress,const std::string& hostname, const std::string& cpuUsage, const std::string& ramUsage) {
    try {
        // Initialize MySQL Connector/C++
        sql::mysql::MySQL_Driver* driver;
        driver = sql::mysql::get_mysql_driver_instance();

        // Establish a connection to the MySQL database
        Connection* con;
        con = driver->connect("tcp://localhost:3306", "root", "");
        con->setSchema("Clients_Info");

        // Create the clients and system info tables if they don't exist
        CreateTables(con);

        // Store client info (hostname) in the clients table if it doesn't exist
        StoreClientInfoInDatabase(con, macaddress);

        // Retrieve the client_id for the given hostname
        PreparedStatement* getClientIdStmt;
        const char* getClientIdSQL = "SELECT client_id FROM Clients WHERE macaddress = ?";
        getClientIdStmt = con->prepareStatement(getClientIdSQL);
        getClientIdStmt->setString(1, macaddress);
        ResultSet* resultSet = getClientIdStmt->executeQuery();

        int clientId = -1; // Initialize to a value that indicates the hostname is not in the table
        if (resultSet->next()) {
            clientId = resultSet->getInt("client_id");
        }

        // Insert system information with the corresponding client_id
        PreparedStatement* insertSystemInfoStmt;
        const char* insertSystemInfoSQL = "INSERT INTO systems_info (client_id,hostname, cpuUsage, ramUsage) VALUES (?,?, ?, ?)";
        insertSystemInfoStmt = con->prepareStatement(insertSystemInfoSQL);
        insertSystemInfoStmt->setInt(1, clientId);
           insertSystemInfoStmt->setString(2,hostname);
        insertSystemInfoStmt->setString(3, cpuUsage);
        insertSystemInfoStmt->setString(4, ramUsage);
        insertSystemInfoStmt->execute();

        // Clean up and close the database connection
        delete getClientIdStmt;
        delete insertSystemInfoStmt;
        delete con;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


// Rest of the HandleClient function remains the same as in the previous code snippet



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
            string macaddress = received_tree.get<string>("macaddress");
            string hostname = received_tree.get<string>("hostname");
            string cpuUsage = received_tree.get<string>("cpu_usage");
            string ramUsage = received_tree.get<string>("ram_usage");
            string modelName = received_tree.get<string>("model_name");

            cout << "\n Received macaddress: " << macaddress << endl;
            cout << "Received hostname: " << hostname << endl;
            cout << "Received CPU Usage: " << cpuUsage << endl;
            cout << "Received RAM Usage: " << ramUsage << endl;
            cout << "Received Model Name: " << modelName << endl;



             StoreSystemInfoInDatabase(macaddress,hostname, cpuUsage, ramUsage);

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
           thread(HandleClient,std::move(socket)).detach() ;
        }
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
