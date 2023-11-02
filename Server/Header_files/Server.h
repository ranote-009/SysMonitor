#pragma once
#include <string>
#include <boost/beast.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <thread>
#include "DatabaseManager.h"

using namespace std;
using namespace sql;
using namespace boost::asio;
namespace pt = boost::property_tree;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace ssl = boost::asio::ssl;

class Server {
public:
    Server(const std::string& connectionKey);
    bool VerifyConnectionKey(websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& ws, const std::string& expectedKey);
    void HandleClient(ssl::stream<tcp::socket>&& stream);

private:
    DatabaseManager db;
    std::string connectionKey_;
};
