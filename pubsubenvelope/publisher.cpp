// publisher.cpp
// Simple pub/sub pattern subscriber demonstrating use of envelopes.

#include <thread>
#include <chrono>
#include <string_view>

#include <zmq.hpp>

void sendmore(zmq::socket_t& s, std::string_view data)
{
    zmq::message_t message{data.cbegin(), data.cend()};
    s.send(message, zmq::send_flags::sndmore);
}

void send(zmq::socket_t& s, std::string_view data)
{
    zmq::message_t message{data.cbegin(), data.cend()};
    s.send(message, zmq::send_flags::none);
}

int main()
{
    using namespace std::chrono_literals;

    zmq::context_t context{1};

    zmq::socket_t publisher{context, ZMQ_PUB};
    publisher.bind("tcp://*:5555");

    for (;;)
    {
        sendmore(publisher, "A");
        send(publisher, "Content tagged with A");

        sendmore(publisher, "B");
        send(publisher, "Content tagged with B");

        std::this_thread::sleep_for(2s);
    }

    return 0;
}