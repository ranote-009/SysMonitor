# SysMonitor

This project is a system for exchanging system information between a server and client over a WebSocket connection. The server is responsible for receiving and processing system information from clients, while clients periodically send system data to the server. The server can also send warning messages to clients when certain conditions are met.

## Project Structure

The project is organized into separate directories for the server and client components, each with its own set of files. Here's an overview of the project structure:

### Certificate Directory

The `certificate` directory contains the SSL certificate and private key used for secure communication between the server and clients. The files in this directory include:

- `server.crt`: The SSL certificate for the server.
- `server.key`: The private key for the server's SSL certificate.

### Client Directory

The `Client` directory contains the client-side code and build configuration:

- `CMakeLists.txt`: The CMake configuration file for building the client.
- `SystemInfoClient.cpp`: The C++ source code for the client application.
- `SystemInfoClient.h`: The header file for the client application.
- `main.cpp`: The main entry point for the client application.
- `server_info.json`: A configuration file that specifies the server's IP address and port. Update this file with your server's details.

### Server Directory

The `Server` directory contains the server-side code and build configuration:

- `Header_files`: A directory containing header files used by the server code.
- `CMakeLists.txt`: The CMake configuration file for building the server.
- `DatabaseManager.cpp`: The C++ source code for managing the database.
- `Server.cpp`: The C++ source code for the server application.
- `exported_data.csv`: A CSV file where server data is exported.
- `main.cpp`: The main entry point for the server application.

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

2. Modify the SSL certificate and private key paths in the `Server` class constructor to match the locations of `server.crt` and `server.key` from the `certificate` directory. This enables secure communication with clients over SSL/TLS.

3. Create a MySQL database named `sys_info` on your MySQL server. You can change the schema name by modifying the `con->setSchema("sys_info")` line in the `DatabaseManager` class constructor.

4. Build the server using CMake. You can do this with the following commands in the `Server` directory:

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

1. Modify the SSL certificate path in the `SystemInfoClient` class constructor to match the location of `server.crt` from the `certificate` directory.

2. Specify the server's IP address, port, log file path, and connection key in the `server_info.json` file(recommended) or in `main` function in the `main.cpp` file within the `Client` directory:

   ```cpp
   SystemInfoClient client("ServerIPAddress", ServerPort, "log.csv", "YourConnectionKeyHere");
   ```

3. Build the client using CMake. You can do this with the following commands in the `Client` directory:

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

The project includes a `server_info.json` configuration file in the `Client` directory. You should copy this file to the build directory when running the client. This file specifies the server's IP address and port. Update these values to match your server configuration.

## Contributing

Contributions to this project are welcome. If you would like to contribute to this project, please follow these steps:

1. Fork the repository.

2. Create a new branch for your feature or bug fix.

3. Make your changes and commit them to your branch.

4. Push your branch to your fork on GitHub.

5. Create a pull request with a detailed description of your changes.



## Contact

For more information about the project or if you have any questions, feel free to contact the author.
