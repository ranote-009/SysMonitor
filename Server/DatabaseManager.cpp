#include "DatabaseManager.h"

DatabaseManager:: DatabaseManager() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect("tcp://localhost:3306", "root", "");
            con->setSchema("sys_info");
            CreateTables(con);
        } catch (const std::exception& e) {
            std::cerr << "DatabaseManager Exception: " << e.what() << std::endl;
        }
    }

    DatabaseManager::~DatabaseManager() {
        if (con) {
            delete con;
        }
    }

    void DatabaseManager::CreateTables(Connection* con) {
        try {
            Statement* stmt = con->createStatement();
            const char* createClientsTableSQL = R"(
                CREATE TABLE IF NOT EXISTS Clients (
                    client_id INT AUTO_INCREMENT PRIMARY KEY,
                    macaddress VARCHAR(255) NOT NULL
                )
            )";
            const char* createSystemInfoTableSQL = R"(
                CREATE TABLE IF NOT EXISTS systems_information (
                    system_info_id INT AUTO_INCREMENT PRIMARY KEY,
                    client_id INT,
                    hostname VARCHAR(255) NOT NULL,
                    cpuUsage VARCHAR(255) NOT NULL,
                    ramUsage VARCHAR(255) NOT NULL,
                    hdd_utilization VARCHAR(255) NOT NULL,
                    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY (client_id) REFERENCES Clients(client_id)
                )
            )";
            stmt->execute(createClientsTableSQL);
            stmt->execute(createSystemInfoTableSQL);
            delete stmt;
        } catch (const std::exception& e) {
            std::cerr << "CreateTables Exception: " << e.what() << std::endl;
        }
    }

    void DatabaseManager::StoreClientInfo(Connection* con, const std::string& macaddress) {
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

    void DatabaseManager::StoreSystemInfo(const std::string& macaddress, const std::string& hostname, const std::string& cpuUsage, const std::string& ramUsage,const std::string& hdd_utilization) {
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
            const char* insertSystemInfoSQL = "INSERT INTO systems_information (client_id, hostname, cpuUsage, ramUsage, hdd_utilization) VALUES (?, ?, ?, ?, ? )";
            insertSystemInfoStmt = con->prepareStatement(insertSystemInfoSQL);
            insertSystemInfoStmt->setInt(1, clientId);
            insertSystemInfoStmt->setString(2, hostname);
            insertSystemInfoStmt->setString(3, cpuUsage);
            insertSystemInfoStmt->setString(4, ramUsage);
             insertSystemInfoStmt->setString(5, hdd_utilization);
            insertSystemInfoStmt->execute();
             // Execute a query using a prepared statement
            pstmt = con->prepareStatement("SELECT * FROM systems_information");
            res = pstmt->executeQuery();

            // Define the CSV file name
            const char* csv_file = "exported_data.csv";

            // Write data to a CSV file
            ofstream file(csv_file);
            file << "client_id,system_info_id,hostname,cpuUsage,ramUsage,hdd_utilization,timestamp" << endl;
            while (res->next()) {
           file << res->getString("client_id") << "," << res->getString("system_info_id")<< "," << res->getString("hostname") << "," << res->getString("cpuUsage")<< "," << res->getString("ramUsage")<< "," << res->getString("hdd_utilization")<< "," << res->getString("timestamp")<< endl; // Replace column1 and column2 with actual column names
            }
           file.close();
            delete getClientIdStmt;
            delete insertSystemInfoStmt;
        } catch (const std::exception& e) {
            std::cerr << "StoreSystemInfo Exception: " << e.what() << std::endl;
        }
    }