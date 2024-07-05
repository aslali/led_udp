#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <iomanip>

using boost::asio::ip::udp;
using namespace std;
const int CONTROLLER_TR_BYTES = 256;

class LedController {
public:
    // Constructor initializes UDP socket and threads for receiving and sending data
    LedController(boost::asio::io_context& io_context, const string& robot_ip, short robot_port, short led_port)
        : socket_(io_context, udp::endpoint(udp::v4(), led_port)), robot_endpoint(boost::asio::ip::make_address(robot_ip), robot_port), all_led_data(600, 0) {
        // Start separate threads for receiving and sending data
        receive_thread = thread(&LedController::receiveData, this);
        send_thread = thread(&LedController::sendData, this);
    }

    ~LedController() {
        if (receive_thread.joinable()) receive_thread.join();
        if (send_thread.joinable()) send_thread.join();
    }


private:
    // Thread function to receive data from the robot
    void receiveData() {
        while (true) {
            array<uint8_t, CONTROLLER_TR_BYTES> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            size_t len = socket_.receive_from(boost::asio::buffer(recv_buf), robot_endpoint, 0, error);
            if (!error || error == boost::asio::error::message_size) {
                // Extract LED data from received buffer. First two bytes show the chunk number and the total chunk number
                for (size_t i = 2; i < len; ++i)
                {
                    all_led_data.push_back(static_cast<unsigned int>(recv_buf[i]));
                }

                // Check if the first two bytes match to indicate end of LED data transmission
                if (recv_buf[0] == recv_buf[1])
                {
                    print_leds_data(); // Print the received LED data
                    all_led_data.clear(); // Clear LED data vector for next transmission
                }
            }
        }
    }

    // Thread function to send periodic heartbeat and log message to the robot pc
    void sendData() {
        while (true) {
            this_thread::sleep_for(chrono::seconds(1));
            uint32_t heartbeat = 0x12345678;  // Example heartbeat value
            string log_message = "LED is disconnected";  // Example log message
            int len_buffer = sizeof(heartbeat) + log_message.length() + 1;
            array<char, CONTROLLER_TR_BYTES> send_buffer;
            send_buffer.fill('-');

            heartbeat_to_char(heartbeat, send_buffer); // Convert heartbeat value to char array
            copy(log_message.begin(), log_message.end(), send_buffer.data() + sizeof(heartbeat));
            send_buffer[len_buffer-1] = '\0';
            socket_.send_to(boost::asio::buffer(send_buffer), robot_endpoint);
        }
    }

    // Function to convert heartbeat value to char array
    void heartbeat_to_char(const uint32_t heartbeat, array<char, CONTROLLER_TR_BYTES>& send_buffer)
    {
        send_buffer[0] = static_cast<char>((heartbeat >> 24) & 0xFF);
        send_buffer[1] = static_cast<char>((heartbeat >> 16) & 0xFF);
        send_buffer[2] = static_cast<char>((heartbeat >> 8) & 0xFF);
        send_buffer[3] = static_cast<char>(heartbeat & 0xFF);
    }

    // Function to print LED data received from the robot
    void print_leds_data()
    {
        for (int i=0; i<all_led_data.size(); ++i)
        {
            // Print RGB values in parentheses
            if (i%3==0)
            {
                cout << "(" << all_led_data[i] << ", "; // R value
            }
            else if (i%3 == 1)
            {
                cout << all_led_data[i] << ", "; // G value
            }
            else
            {
                cout << all_led_data[i] << ") "; // B vvalue
            }
        }
        cout << endl << endl << endl;
    }

    udp::socket socket_;
    udp::endpoint robot_endpoint;
    thread receive_thread;
    thread send_thread;
    vector<unsigned> all_led_data;
};

int main() {
    try {
        boost::asio::io_context io_context;
        // Two different ports for receiving and sending data between the robot_pc and the led controller.
        // The ips are the same as we run them on the same computer (local address)
        LedController led(io_context, "127.0.0.1", 12345, 12346);
        io_context.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
