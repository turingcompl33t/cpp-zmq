// client.cpp
// Extension of simple request/reply server that supports
// an intermediary broker.

#include <string>
#include <iostream>

#include <zmq.hpp>

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t socket{context, ZMQ_REQ};
    socket.connect("tcp://localhost:5555");

    const std::string data{"Hello"};

    for (auto request_num = 0; request_num < 10; ++request_num) 
    {
        // initialize a request message
        zmq::message_t request{data.cbegin(), data.cend()};
        
        // send the request message
        socket.send(request, zmq::send_flags::none);
        
        // wait for reply from server
        zmq::message_t reply{};
        socket.recv(reply, zmq::recv_flags::none);

        std::cout << "Received " << reply.to_string();
        std::cout << std::endl;
    }

    return 0;
}