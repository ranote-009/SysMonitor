// main.cpp
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "SystemInfoClient.h"

int main() {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("/home/abhsihek/SysMonitor/SysMonitor/Client/server_info.json", pt);

    // Extract IP address and port
    std::string ipAddress = pt.get<std::string>("ip_address");
    int port = pt.get<int>("port");
    SystemInfoClient client(ipAddress, port, "/home/abhsihek/SysMonitor/SysMonitor/Client/build/log.csv","CsGo@2023");
    client.run();

    return 0;
}
