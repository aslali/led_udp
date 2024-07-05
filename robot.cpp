 /*The LED controller receives UDP packets from the robot_pc containing LED data,
 where each packet includes a header indicating its sequential order (chunk_index)
 and the total number of packets (total_chunks) expected for the complete data set.
 Upon reception, the controller parses these headers to ensure packets are processed
 in the correct order, preventing data misalignment. It then extracts and accumulates
 the LED data from each packet's payload, reconstructing the complete LED data stream.*/

#include <iostream>
#include <boost/asio.hpp>
#include "robot_pc.hpp"
using namespace std;

int main() {
    try {
        boost::asio::io_context io_context;
        // Two different ports for receiving and sending data between the robot_pc and the led controller.
        // The ips are the same as we run them on the same computer (local address)
        RobotPC robotpc(io_context, 12345, 12346, "127.0.0.1");
        // Example usage to set LEDs color. I only considered two cases stop (all leds are red) and default (all leds are off). More cases can be added.
        robotpc.set_leds_color("stop");
        io_context.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
