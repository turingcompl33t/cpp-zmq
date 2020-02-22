// publisher.cpp
// Basic publisher-subscriber pattern server.

#include <thread>
#include <chrono>
#include <iostream>

#include <zmq.hpp>

int main()
{
    using namespace std::chrono_literals;

    zmq::context_t context{1};
    zmq::socket_t publisher{context, ZMQ_PUB};
    publisher.bind("tcp://*:5555");

    for (;;)
    {
        zmq::message_t msg{17};
        memcpy((char*)msg.data(), "Hello Subscribers", 17);

        publisher.send(msg, zmq::send_flags::none);

        std::this_thread::sleep_for(3s);
    }

    return 0;
}