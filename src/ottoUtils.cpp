#include "ottoUtils.h"
#include <chrono>
#include <iostream>
using namespace std;
using boost::asio::ip::udp;

namespace ottoUtils
{
    vector<uint8_t> set_leds_color(const int led_count, string mode) // This functions only set the leds color, but does not transfer it to the robot.
    {
        vector<uint8_t> leds_data(led_count*3, 0);
        if (mode=="stop") // I assume that when the robot is in the "stop" mode, all leds turn into red.
        {
            for (int i=0; i<led_count; i++)
            {
                leds_data[3*i] = 255;
                leds_data[3*i+1] = 0;
                leds_data[3*i+2] = 0;
            }
        }

            // More cases can be added here.

        else // I assume that in the default mode the robot's leds are off.
        {
            for (int i=0; i<led_count; i++)
            {
                leds_data[3*i] = 0;
                leds_data[3*i+1] = 0;
                leds_data[3*i+2] = 0;
            }
        }
        return leds_data;
    }




    LEDServer::LEDServer(boost::asio::io_context& io_context, const string ip_address, int port)
        : io_context_(io_context), socket_(io_context, udp::endpoint(udp::v4(), 0)),
          endpoint_(boost::asio::ip::make_address(ip_address), port) {}
//    LEDServer::LEDServer(boost::asio::io_context& io_context, const string ip_address, int port)
//    {
//        int lfdfdhf=1;
//    }

    void LEDServer::createServer() {
        // Start sender and receiver threads
        sender_thread_ = thread(&LEDServer::senderThread, this);
        receiver_thread_ = thread(&LEDServer::receiverThread, this);
    }

    void LEDServer::run() {
        // Join sender and receiver threads
        sender_thread_.join();
        receiver_thread_.join();
    }


    // Thread function for sending LED data
    void LEDServer::senderThread() {
        while (true) {
            auto start_time = std::chrono::steady_clock::now();
            send_led_data();
            auto end_time = std::chrono::steady_clock::now();
            std::this_thread::sleep_until(start_time + chrono::milliseconds(20));
        }
    }

    // Thread function for receiving data
    void LEDServer::receiverThread() {
        int fdfdfd=3;
//        receiveData();
    }


    void LEDServer::send_led_data() {
        auto data = set_leds_color(200, "stop");
        int buffer_size = 256;
        int total_chunks = data.size()/ (buffer_size - 2);
        vector <int> packetLenghts(total_chunks, buffer_size - 2);
        if (data.size()%(buffer_size - 2)!=0)
        {
            total_chunks++;
            packetLenghts.push_back(data.size()%(buffer_size - 2));
        }
        for (auto pp:packetLenghts)
        {
            cout << pp << endl;
        }
        int spoint = 0;
        for (int chunk_index = 0; chunk_index < total_chunks; chunk_index++) {

            std::vector<uint8_t> packet(256);
            packet[0] = static_cast<uint8_t>(chunk_index);
            packet[1] = static_cast<uint8_t>(total_chunks);


            cout << spoint<< endl;
            copy(data.begin() + spoint, data.begin() + spoint + packetLenghts[chunk_index], packet.begin() + 2);

//            socket_.send_to(boost::asio::buffer(packet, packetLenghts[chunk_index] + 2), endpoint_);
            spoint += packetLenghts[chunk_index];
        }
    }



}


