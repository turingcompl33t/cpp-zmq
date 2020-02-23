// broker.cpp
// Broker application for extended request/reply pattern.

#include <vector>
#include <iostream>

#include <zmq.hpp>

using poll_vector_t = std::vector<zmq::pollitem_t>;

int main()
{
    zmq::context_t context{1};
    
    zmq::socket_t frontend{context, ZMQ_ROUTER};  // for REQ sockets
    zmq::socket_t backend{context, ZMQ_DEALER};   // for REP sockets

    frontend.bind("tcp://*:5555");
    backend.bind("tcp://*:5556");

    poll_vector_t items{};
    items.push_back({static_cast<void*>(frontend), 0, ZMQ_POLLIN, 0});
    items.push_back({static_cast<void*>(backend), 0, ZMQ_POLLIN, 0});

    for (;;)
    {
        zmq::message_t msg;

        zmq::poll(items, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            for (;;)
            {
                frontend.recv(msg, zmq::recv_flags::none);
                auto const more = msg.more();

                backend.send(msg, more 
                    ? zmq::send_flags::sndmore 
                    : zmq::send_flags::none
                    );
                
                if (!more)
                    break;
            }
        }

        if (items[1].revents & ZMQ_POLLIN)
        {
            for (;;)
            {
                backend.recv(msg, zmq::recv_flags::none);
                auto const more = msg.more();

                frontend.send(msg, more
                    ? zmq::send_flags::sndmore
                    : zmq::send_flags::none
                    );
                
                if (!more)
                    break;
            }
        }
    }

    return 0;
}