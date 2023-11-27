#include "./Header_files/Server.h"

Server::Server(const std::string& connectionKey) : connectionKey_(connectionKey) {
    try {
        ssl::context ctx{ssl::context::tlsv12};

        // Load your SSL certificate and private key
        ctx.use_certificate_file("../../Certificate/server.crt", ssl::context::file_format::pem);
        ctx.use_private_key_file("../../Certificate/server.key", ssl::context::file_format::pem);

        net::io_context ioc(1);
        tcp::acceptor acceptor(ioc, {{}, 8080});

        cout << "Server started and waiting for connections..." << endl;

        while (1) {
            ssl::stream<tcp::socket> stream(ioc, ctx);
            acceptor.accept(stream.lowest_layer());
            stream.handshake(ssl::stream_base::server);

            std::thread(&Server::HandleClient, this, std::move(stream)).detach();
        }
    } catch (const std::exception& e) {
        cerr << "Server Exception: " << e.what() << endl;
    }
}

    bool Server::VerifyConnectionKey(websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& ws, const std::string& expectedKey) {
        char key_data[1024];
        boost::system::error_code read_error;
        size_t key_length = ws.read_some(buffer(key_data), read_error);

        if (read_error) {
            cerr << "Error reading connection key: " << read_error.message() << endl;
            return false;
        }

        string receivedKey(key_data, key_length);

        if (receivedKey == expectedKey) {
            cout << " \n \nConnection key is valid. Client is authorized.\n" << endl;
            return true;
        } else {
            cerr << "Invalid connection key. Closing the connection." << endl;
            return false;
        }
    }


    void Server::HandleClient(ssl::stream<tcp::socket>&& stream) {
        try {
             
            cout << "\n \n \nClient connected: " << stream.lowest_layer().remote_endpoint() << endl;

            websocket::stream<ssl::stream<tcp::socket>> ws(std::move(stream));
             ws.accept();

            // Perform WebSocket handshake
            beast::error_code ec;

            ws.next_layer().handshake(ssl::stream_base::server, ec);
            if (ec) {
                cerr << "WebSocket handshake failed: " << ec.message() << endl;
                return;
            }

              if (VerifyConnectionKey(ws, connectionKey_)) {
                  //  thread(&Server::HandleClient, this, std::move(socket)).detach();
                } else {
                    beast::error_code ec;
                   ws.close(websocket::close_code::normal, ec);
                   if (ec) {
                       std::cerr << "Error while closing websocket: " << ec.message() << std::endl;
                 }
                    cout << "Unauthorized client disconnected." << endl;
                }


            while (1) {
                beast::flat_buffer buffer;
                beast::error_code read_ec;
                ws.read(buffer, read_ec);
                if (read_ec) {
                    cerr << "WebSocket read error: " << read_ec.message() << endl;
                    return;
                }

                string received_data(beast::buffers_to_string(buffer.data()));
                cout << "\nReceived " << buffer.size() << " bytes of data from the client" << endl;

                try {
                    pt::ptree received_tree;
                    istringstream received_stream(received_data);
                    pt::read_json(received_stream, received_tree);

                    string macaddress = received_tree.get<string>("macaddress");
                    string hostname = received_tree.get<string>("hostname");
                    string cpuUsage = received_tree.get<string>("cpu_usage");
                    string ramUsage = received_tree.get<string>("ram_usage");
                    string hdd_utilization = received_tree.get<string>("hdd_utilization");

                    cout << "\nReceived macaddress: " << macaddress << endl;
                    cout << "Received hostname: " << hostname << endl;
                    cout << "Received CPU Usage: " << cpuUsage << endl;
                    cout << "Received RAM Usage: " << ramUsage << endl;
                    cout << "Received HDD Utilization Name: " << hdd_utilization << endl;


                // Warning analysis       
                     hdd_utilization.pop_back();
                   // Converting the string to an integer
                     int inthdd = std::stoi(hdd_utilization);
                   int ramUsagePercentage = (stoi(ramUsage)*100) / stoi(ramUsage.substr(15,4));

                    // Send a warning response to the client
                    if ( stoi(cpuUsage) > 5){

                        string warningResponse = "CPU USAGE IS MORE THAN 90% !!!";
                        beast::error_code write_ec;
                        ws.write(boost::asio::buffer(warningResponse), write_ec);
                        if (write_ec) {
                            cerr << "Unable to send warning\n" << "WebSocket write error: " << write_ec.message() << endl;
                            return;
                        } else {
                            cout << "Sent CPU warning  response to the client" << "(" << macaddress << ")" << endl;
                        }

                    }
                     if ( ramUsagePercentage>90){

                        string warningResponse = "RAM USAGE IS MORE THAN 90% !!!";
                        beast::error_code write_ec;
                        ws.write(boost::asio::buffer(warningResponse), write_ec);
                        if (write_ec) {
                            cerr << "Unable to send warning\n" << "WebSocket write error: " << write_ec.message() << endl;
                            return;
                        } else {
                            cout << "Sent RAM warning  response to the client" << "(" << macaddress << ")" << endl;
                        }

                    }
                     if ( inthdd>90){

                        string warningResponse = "HDD USAGE IS MORE THAN 90% !!!";
                        beast::error_code write_ec;
                        ws.write(boost::asio::buffer(warningResponse), write_ec);
                        if (write_ec) {
                            cerr << "Unable to send warning\n" << "WebSocket write error: " << write_ec.message() << endl;
                            return;
                        } else {
                            cout << "Sent  HDD warning  response to the client" << "(" << macaddress << ")" << endl;
                        }

                    }


                    try {
                        // Your existing code for storing data in the database
                        db.StoreSystemInfo(macaddress, hostname, cpuUsage, ramUsage,hdd_utilization);
                    } catch (const std::exception& e) {
                        cerr << "Database Exception: " << e.what() << endl;
                        return;
                    }

                    if(!(stoi(cpuUsage) > 90||ramUsagePercentage>90 || inthdd>90)) {
                          // Send a success response back to the client
                    string successResponse = "Data transfer was successful!";
                    beast::error_code write_ec;
                    ws.write(boost::asio::buffer(successResponse), write_ec);
                    if (write_ec) {
                        cerr << "WebSocket write error: " << write_ec.message() << endl;
                        return;
                    } else {
                        cout << "Sent success response to the client" << endl;
                    }

                    }
                  
                   
                } catch (const std::exception& e) {
                    cerr << "JSON Parsing Exception: " << e.what() << endl;
                    return;
                }
            }
        } catch (const exception& e) {
            cerr << "HandleClient Exception: " << e.what() << endl;
        }
    }