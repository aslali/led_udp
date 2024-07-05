#ifndef ROBOTPC_H
#define ROBOTPC_H

#include <boost/asio.hpp>
#include <thread>
#include <vector>
using namespace std;

const int LED_COUNT = 200;
const int LEDS_RGB_BYTES = 3 * LED_COUNT;
const int CONTROLLER_TR_BYTES = 256;
const int COMMUNICATION_INTERVAL = 100;

class RobotPC {
public:
    RobotPC(boost::asio::io_context& io_context, short robot_port, short led_port, string robot_ip);

    ~RobotPC();

    vector<uint8_t> set_leds_color(string mode);

private:
    void receiveData();

    void sendData();

    void send_led_data();

    uint32_t char_to_heartbeat(array<char, CONTROLLER_TR_BYTES>& recv_buf);

    vector<uint8_t> leds_data;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint led_endpoint;
    thread receive_thread;
    thread send_thread;
};

#endif // ROBOTPC_H
