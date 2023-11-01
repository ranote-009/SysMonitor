// SystemInfoClient.hpp
#ifndef SYSTEM_INFO_CLIENT_H
#define SYSTEM_INFO_CLIENT_H

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp> 
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <thread>


namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ssl = asio::ssl; // SSL namespace
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace pt = boost::property_tree;
using namespace std;

class SystemInfoClient {
public:
    SystemInfoClient(const std::string& serverAddress, int serverPort, const std::string& logFilePath, const std::string& connectionKey);
    void run();

private:
    std::string serverAddress_;
    int serverPort_;
    std::string logFilePath_;
    int reconnectAttempts_;
    std::string connectionKey_;
    asio::io_context ioc_;
    ssl::context ctx{ssl::context::tlsv12_client};
   websocket::stream<beast::ssl_stream<asio::ip::tcp::socket>> ws_;
    beast::flat_buffer buffer_;

    // Add private methods for sending, receiving data, and logging
    void sendKey();
    std::string exec(const char* cmd);
    void connect();
    void sendSystemInfo();
    void receiveResponse();
    void logSuccess(std::string result);
};

#endif // SYSTEM_INFO_CLIENT_HPP
