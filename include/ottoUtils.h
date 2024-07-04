#ifndef OTTOUTILS_H
#define OTTOUTILS_H
#include <vector>
#include <string>
#include <thread>

#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::udp;

namespace ottoUtils
{
    vector<uint8_t> set_leds_color (const int led_count, string mode);

    class LEDServer
    {
        public:
            LEDServer(boost::asio::io_context& io_context, const string ip_address, int port);
            void createServer();
            void run();
            void send_led_data();
        private:
        boost::asio::io_context& io_context_;
        udp::socket socket_;
        udp::endpoint endpoint_;
        thread sender_thread_;
        thread receiver_thread_;

        void senderThread();
        void receiverThread();

    };
};

#endif // OTTOUTILS_H
