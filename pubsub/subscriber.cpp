// subscriber.cpp
// Basic publisher-subscriber pattern example.

#include <iostream>

#include <zmq.hpp>

int main()
{
    zmq::context_t context{1};
    zmq::socket_t socket{context, ZMQ_SUB};
    
    socket.connect("tcp://localhost:5555");
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    std::cout << "Reading messages on tcp://localhost:5555...\n";

    for (;;)
    {
        zmq::message_t msg{};
        socket.recv(msg, zmq::recv_flags::none);

        std::cout << "[RECV] " << msg.to_string() << std::endl;
    }
}