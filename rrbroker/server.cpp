// server.cpp
// Extension of simple request/reply server that supports
// an intermediary broker.

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#include <zmq.hpp>

int main() 
{
    using namespace std::chrono_literals;

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REP (reply) socket and bind to interface
    zmq::socket_t socket{context, ZMQ_REP};
    socket.connect("tcp://127.0.0.1:5556");

    const std::string data{"World"};

    for (;;) 
    {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);
        std::cout << "Received " << request.to_string(); 
        std::cout << std::endl;

        // simulate work
        std::this_thread::sleep_for(1s);

        // construct a reply message
        zmq::message_t reply{data.cbegin(), data.cend()};

        // send the reply to the client
        socket.send(reply, zmq::send_flags::none);
    }

    return 0;
}