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

class DatabaseManager {
public:
    DatabaseManager() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect("tcp://localhost:3306", "root", "");
            con->setSchema("testdb");
            CreateTables(con);
        } catch (const std::exception& e) {
            std::cerr << "DatabaseManager Exception: " << e.what() << std::endl;
        }
    }

    ~DatabaseManager() {
        if (con) {
            delete con;
        }
    }

    void CreateTables(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            const char* createClientsTableSQL = R"(
                CREATE TABLE IF NOT EXISTS Clients (
                    client_id INT AUTO_INCREMENT PRIMARY KEY,
                    macaddress VARCHAR(255) NOT NULL
                )
            )";
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
            stmt->execute(createClientsTableSQL);
            stmt->execute(createSystemInfoTableSQL);
            delete stmt;
        } catch (const std::exception& e) {
            std::cerr << "CreateTables Exception: " << e.what() << std::endl;
        }
    }

    void StoreClientInfo(Connection* con,const std::string& macaddress) {
        try {
            PreparedStatement* checkClientStmt;
            const char* checkClientSQL = "SELECT client_id FROM Clients WHERE macaddress = ?";
            checkClientStmt = con->prepareStatement(checkClientSQL);
            checkClientStmt->setString(1, macaddress);
            ResultSet* resultSet = checkClientStmt->executeQuery();
            int clientId = -1;
            if (resultSet->next()) {
                clientId = resultSet->getInt("client_id");
            }
            if (clientId == -1) {
                PreparedStatement* insertClientStmt;
                const char* insertClientSQL = "INSERT INTO Clients (macaddress) VALUES (?)";
                insertClientStmt = con->prepareStatement(insertClientSQL);
                insertClientStmt->setString(1, macaddress);
                insertClientStmt->execute();
                clientId = insertClientStmt->getUpdateCount();
                delete insertClientStmt;
            }
            delete checkClientStmt;
        } catch (const std::exception& e) {
            std::cerr << "StoreClientInfo Exception: " << e.what() << std::endl;
        }
    }

    void StoreSystemInfo(const std::string& macaddress, const std::string& hostname, const std::string& cpuUsage, const std::string& ramUsage) {
        try {
               CreateTables(con);
            StoreClientInfo(con, macaddress);
            PreparedStatement* getClientIdStmt;
            const char* getClientIdSQL = "SELECT client_id FROM Clients WHERE macaddress = ?";
            getClientIdStmt = con->prepareStatement(getClientIdSQL);
            getClientIdStmt->setString(1, macaddress);
            ResultSet* resultSet = getClientIdStmt->executeQuery();
            int clientId = -1;
            if (resultSet->next()) {
                clientId = resultSet->getInt("client_id");
            }
            PreparedStatement* insertSystemInfoStmt;
            const char* insertSystemInfoSQL = "INSERT INTO systems_info (client_id, hostname, cpuUsage, ramUsage) VALUES (?, ?, ?, ?)";
            insertSystemInfoStmt = con->prepareStatement(insertSystemInfoSQL);
            insertSystemInfoStmt->setInt(1, clientId);
            insertSystemInfoStmt->setString(2, hostname);
            insertSystemInfoStmt->setString(3, cpuUsage);
            insertSystemInfoStmt->setString(4, ramUsage);
            insertSystemInfoStmt->execute();
            delete getClientIdStmt;
            delete insertSystemInfoStmt;
        } catch (const std::exception& e) {
            std::cerr << "StoreSystemInfo Exception: " << e.what() << std::endl;
        }
    }

private:
    sql::mysql::MySQL_Driver* driver;
    Connection* con;
};

class Server {
public:
    Server() {
        try {
            io_service service;
            tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 3000));
            cout << "Server started and waiting for connections..." << endl;

            while (1) {
                tcp::socket socket(service);
                acceptor.accept(socket);
                thread(&Server::HandleClient, this, std::move(socket)).detach();
            }
        } catch (const exception& e) {
            cerr << "Server Exception: " << e.what() << endl;
        }
    }

    void HandleClient(tcp::socket&& socket) {
        try {
            cout << "Client connected: " << socket.remote_endpoint() << endl;

            while (1) {
                char data[1024];
                boost::system::error_code error;
                size_t len = socket.read_some(buffer(data));
                string received_data(data, len);
                cout << "Received " << len << " bytes of data from the client" << endl;

                pt::ptree received_tree;
                istringstream received_stream(received_data);
                pt::read_json(received_stream, received_tree);

                string macaddress = received_tree.get<string>("macaddress");
                string hostname = received_tree.get<string>("hostname");
                string cpuUsage = received_tree.get<string>("cpu_usage");
                string ramUsage = received_tree.get<string>("ram_usage");
                string modelName = received_tree.get<string>("model_name");

                cout << "\nReceived macaddress: " << macaddress << endl;
                cout << "Received hostname: " << hostname << endl;
                cout << "Received CPU Usage: " << cpuUsage << endl;
                cout << "Received RAM Usage: " << ramUsage << endl;
                cout << "Received Model Name: " << modelName << endl;

                db.StoreSystemInfo(macaddress, hostname, cpuUsage, ramUsage);

                string successResponse = "Data transfer was successful!";
                socket.write_some(buffer(successResponse));
                cout << "Sent success response to the client" << endl;
            }

            socket.close();
            cout << "Client disconnected: " << socket.remote_endpoint() << endl;
        } catch (const exception& e) {
            cerr << "HandleClient Exception: " << e.what() << endl;
        }
    }

private:
    DatabaseManager db;
};

int main() {
    Server server;
    return 0;
}
