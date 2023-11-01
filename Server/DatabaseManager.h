#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

using namespace std;
using namespace sql;
sql::PreparedStatement *pstmt;
sql::ResultSet *res;

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    void CreateTables(sql::Connection* con);
    void StoreClientInfo(sql::Connection* con, const std::string& macaddress);
    void StoreSystemInfo(const std::string& macaddress, const std::string& hostname, const std::string& cpuUsage, const std::string& ramUsage, const std::string& hdd_utilization);

private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
};
