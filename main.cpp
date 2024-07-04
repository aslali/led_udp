#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <vector>
#include <array>
#include <ottoUtils.h>

using namespace std;

const int LED_COUNT = 200;
const int LEDS_RGB_BYTES = 3 * LED_COUNT; // 24-bit colour space = 3 byte
const int CONTROLLER_TR_BYTES = 256;
const int COMMUNICATION_INTERVAL = 0.1; // 10Hz == 100 ms



int main()
{
    boost::asio::io_context io_context;
    ottoUtils::LEDServer server(io_context, "192.168.0.100", 12345);
    server.send_led_data();

//    server.createServer();
//    server.run();

    return 0;

}
