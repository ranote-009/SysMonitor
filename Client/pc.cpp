#include <iostream>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

class SystemInfoClient {
public:
    SystemInfoClient(const std::string& serverAddress, int serverPort)
        : serverAddress_(serverAddress), serverPort_(serverPort), socket_(ioService_) {
    }

    void sendSystemInfo() {
        // Simulating system information data
        std::string systemInfo = "Sample system information data";

        // Establish connection
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(serverAddress_), serverPort_);
        socket_.connect(endpoint);

        // Send system information data
        boost::system::error_code ignored_error;
        boost::asio::write(socket_, boost::asio::buffer(systemInfo), ignored_error);

        // Set a timer for receiving data
        deadline_.expires_from_now(boost::posix_time::seconds(5)); // Adjust the timeout as needed
        deadline_.async_wait([this](const boost::system::error_code& error) {
            if (error) {
                if (error != boost::asio::error::operation_aborted) {
                    std::cout << "Timeout occurred." << std::endl;
                    socket_.close();
                }
            }
        });

        // Wait for the response
        boost::asio::streambuf receiveBuffer;
        boost::asio::read_until(socket_, receiveBuffer, "\n");
        std::string responseMessage = boost::asio::buffer_cast<const char*>(receiveBuffer.data());
        std::cout << "Received response from the server: " << responseMessage << std::endl;

        // Close the socket when done
        socket_.close();
    }

private:
    boost::asio::io_service ioService_;
    tcp::socket socket_;
    boost::asio::deadline_timer deadline_;
    std::string serverAddress_;
    int serverPort_;
};

int main() {
    SystemInfoClient client("127.0.0.1", 3000); // Change the server IP address and port as needed
    client.sendSystemInfo();

    return 0;
}
