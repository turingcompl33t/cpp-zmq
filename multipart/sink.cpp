// sink.cpp
// A simple example that pulls a single multipart message.

#include <sstream>
#include <iostream>

#include <zmq.hpp>

int main()
{
    zmq::context_t context{1};

    zmq::socket_t sink{context, ZMQ_REP};
    sink.bind("tcp://*:5555");

    std::ostringstream stream{};

    for (;;)
    {
        zmq::message_t msg;

        // recv the message
        sink.recv(msg, zmq::recv_flags::none);
        
        // process the message
        stream << msg.to_string();

        if (!msg.more())
            break;
    }

    std::cout << "Reconstructed Message:\n";
    std::cout << stream.str();

    return 0;
}