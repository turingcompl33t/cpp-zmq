// inproc.cpp
// Simple in-process client /server with cppzmq.

#include <vector>
#include <iostream>

#include <zmq_addon.hpp>

//#pragma comment(lib, "libzmq-v142-mt-gd-4_3_2.lib")

int main()
{
    zmq::context_t ctx;

    zmq::socket_t client(ctx, zmq::socket_type::pair);
    zmq::socket_t server(ctx, zmq::socket_type::pair);

    client.bind("inproc://test");
    server.connect("inproc://test");

    std::array<zmq::const_buffer, 2> send_msgs = {
        zmq::str_buffer("foo"),
        zmq::str_buffer("bar!")
    };

    if (!zmq::send_multipart(client, send_msgs))
        return 1;

    std::vector<zmq::message_t> recv_msgs;
    const auto ret = zmq::recv_multipart(
        server, std::back_inserter(recv_msgs));

    if (!ret)
        return 1;

    std::cout << "Got " << *ret
              << " messages" << std::endl;
    
    return 0;
}