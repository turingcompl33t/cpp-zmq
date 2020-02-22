// hwserver.cpp
// Hello World server in C++
// Binds REP socket to tcp://*:5555
// Expects "Hello" from client, replies with "World"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <zmq.hpp>

int main() 
{
    using namespace std::chrono_literals;

    zmq::context_t context(1);
    zmq::socket_t socket{context, ZMQ_REP};
    socket.bind("tcp://*:5555");

    while (true) 
    {
        zmq::message_t request;

        socket.recv(request, zmq::recv_flags::none);
        std::cout << "Received Hello" << std::endl;

        // simulate work
        std::this_thread::sleep_for(1s);

        zmq::message_t reply(5);
        memcpy(reply.data (), "World", 5);
        socket.send(reply, zmq::send_flags::none);
    }

    return 0;
}