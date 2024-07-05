#include "robot_pc.hpp"
#include <iostream>
#include <iomanip>


// Constructor initializes UDP socket and threads for receiving and sending data
RobotPC::RobotPC(boost::asio::io_context& io_context, short robot_port, short led_port, string robot_ip)
    : socket(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), robot_port)),
      leds_data(LEDS_RGB_BYTES, 0),
      led_endpoint(boost::asio::ip::make_address(robot_ip), led_port)
{
    // Start separate threads for receiving and sending data
    receive_thread = thread(&RobotPC::receiveData, this);
    send_thread = thread(&RobotPC::sendData, this);
}

// Function to set LED colors based on mode ("stop" or default)
vector<uint8_t> RobotPC::set_leds_color(string mode)
{
    if (mode == "stop") {
        for (int i = 0; i < LED_COUNT; i++) {
            leds_data[3 * i] = 255;
            leds_data[3 * i + 1] = 0;
            leds_data[3 * i + 2] = 0;
        }
    } else {
        for (int i = 0; i < LED_COUNT; i++) {
            leds_data[3 * i] = 0;
            leds_data[3 * i + 1] = 0;
            leds_data[3 * i + 2] = 0;
        }
    }
    return leds_data;
}

RobotPC::~RobotPC() {
    if (receive_thread.joinable()) receive_thread.join();
    if (send_thread.joinable()) send_thread.join();
}

// Thread function to receive data from the LED controller
void RobotPC::receiveData() {
    while (true) {
        array<char, CONTROLLER_TR_BYTES> recv_buf;// Receive buffer for incoming data
        boost::system::error_code error;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), led_endpoint, 0, error);

        if (!error || error == boost::asio::error::message_size) {
            // Extract and print heartbeat value from received data
            cout << "Received heartbeat value: 0x" << hex << setw(8) << char_to_heartbeat(recv_buf) << endl;

            // Print log data received from the LED controller
            cout << "Log data is: ";
            for (auto it = recv_buf.begin() + 4; it < recv_buf.end(); ++it) {
                cout << *it;
            }
            cout << endl;
        }
    }
}

// Thread function to send LED data to the LED controller
void RobotPC::sendData() {
    while (true) {
        auto start_time = chrono::steady_clock::now(); // Record start time for communication interval
        send_led_data(); // Send LED data to the LED controller
        // Wait until next communication interval
        this_thread::sleep_until(start_time + chrono::milliseconds(COMMUNICATION_INTERVAL));
    }
}

// Function to send LED data to the LED controller in chunks.
// In each chunk the first byte shows the current chunk number,
// and the second byte shows the total number of chunks.
void RobotPC::send_led_data() {
    boost::system::error_code error_obj;
    int total_chunks = leds_data.size() / (CONTROLLER_TR_BYTES - 2); // Calculate total number of data chunks to send
    vector<int> packetLengths(total_chunks, CONTROLLER_TR_BYTES - 2); // Vector to store packet lengths
    // Using all 256byte for sending each chunk of data and send remaining data (less than 256 byte) as an additional chunk
    if (leds_data.size() % (CONTROLLER_TR_BYTES - 2) != 0) {
        total_chunks++;
        packetLengths.push_back(leds_data.size() % (CONTROLLER_TR_BYTES - 2));
    }

    int spoint = 0;
    for (int chunk_index = 0; chunk_index < total_chunks; chunk_index++) {
        vector<uint8_t> packet(packetLengths[chunk_index] + 2, 9);
        packet[0] = static_cast<uint8_t>(chunk_index + 1); // Set chunk index
        packet[1] = static_cast<uint8_t>(total_chunks); // Set total number of chunks
        copy(leds_data.begin() + spoint, leds_data.begin() + spoint + packetLengths[chunk_index], packet.begin() + 2); // Copy LED data to packet
        socket.send_to(boost::asio::buffer(packet), led_endpoint, 0, error_obj);
        spoint += packetLengths[chunk_index]; // Move to the next data chunk
    }
}

// Function to convert a portion of received data to a heartbeat value
uint32_t RobotPC::char_to_heartbeat(array<char, CONTROLLER_TR_BYTES>& recv_buf)
{
    uint32_t heartbeat_value = static_cast<unsigned char>(recv_buf[0]) << 24 |
                           static_cast<unsigned char>(recv_buf[1]) << 16 |
                           static_cast<unsigned char>(recv_buf[2]) << 8 |
                           static_cast<unsigned char>(recv_buf[3]);
    return heartbeat_value;
}

