# SysMonitor

This project is a system for exchanging system information between a server and client over a WebSocket connection. The server is responsible for receiving and processing system information from clients, while clients periodically send system data to the server. The server can also send warning messages to clients when certain conditions are met.

## Server (C++)

The server application is implemented in C++ and uses the CMake build system along with various libraries, including Boost, MySQL, OpenSSL, and others. It serves as a WebSocket server that listens on a specified port and processes incoming connections.

### Prerequisites

Before setting up and running the server, you need to make sure you have the following prerequisites:

- C++ compiler
- CMake (version 3.22 or higher)
- Boost C++ libraries
- MySQL server
- OpenSSL (for SSL/TLS support)

### Server Setup

To set up the server:

1. Ensure that you have the necessary prerequisites installed on your server machine.

2. Modify the SSL certificate and private key paths in the `Server` class constructor to match your server's SSL certificate configuration. This enables secure communication with clients over SSL/TLS.

3. Create a MySQL database named `sys_info` on your MySQL server. You can change the schema name by modifying the `con->setSchema("sys_info")` line in the `DatabaseManager` class constructor.

4. Build the server using CMake. You can do this with the following commands:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

5. Run the server by executing the generated server binary.

   ```bash
   ./server
   ```

### Database Schema

The server uses a MySQL database with the following schema:

- `Clients` table: Stores client information, including a unique client identifier and MAC address.
- `systems_information` table: Stores system data, including the client identifier, hostname, CPU usage, RAM usage, HDD utilization, and timestamp.

### Server Behavior

The server has the following behavior:

- Listens for WebSocket connections on port 8080.
- Authenticates clients using a connection key provided during WebSocket handshake.
- Monitors system data sent by clients and stores it in the database.
- Sends warning messages to clients if certain conditions are met, such as high CPU usage.

## Client (C++)

The client application is also implemented in C++ and uses the CMake build system to build the project. It runs on client systems to collect and send system information to the server.

### Prerequisites

Before setting up and running the client, make sure you have the following prerequisites:

- C++ compiler
- CMake (version 3.22 or higher)
- Boost C++ libraries
- OpenSSL (for SSL/TLS support)

### Client Setup

To set up the client:

1. Modify the SSL certificate path in the `SystemInfoClient` class constructor to match your server's SSL certificate.

2. Specify the server's IP address, port, log file path, and connection key in the `main` function.

   ```cpp
   SystemInfoClient client("ServerIPAddress", ServerPort, "log.csv", "YourConnectionKeyHere");
   ```

3. Build the client using CMake. You can do this with the following commands:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Run the client by executing the generated client binary.

   ```bash
   ./client
   ```

### Client Behavior

The client has the following behavior:

- Collects system information, including MAC address, hostname, CPU usage, RAM usage, and HDD utilization.

- Sends this information to the server over a WebSocket connection every 5 seconds.

- Logs the results in a CSV file for future reference.

- Automatically attempts to reconnect to the server if the connection is lost.

## Configuration Files

The project includes a `server_info.json` configuration file that you should copy to the build directory when running the client. This file specifies the server's IP address and port. You can modify these values to match your server configuration.

## Contributing

Contributions to this project are welcome. If you would like to contribute to this project, please follow these steps:

1. Fork the repository.

2. Create a new branch for your feature or bug fix.

3. Make your changes and commit them to your branch.

4. Push your branch to your fork on GitHub.

5. Create a pull request with a detailed description of your changes.



## Contact

For more information about the project or if you have any questions, feel free to contact the author.
```

You can copy and paste this into your GitHub README editor. Make sure to replace the placeholders like `ServerIPAddress`, `YourConnectionKeyHere`, and other values with your actual configuration.

   
