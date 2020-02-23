// autobroker.cpp
// Broker application for extended request/reply pattern;
// the "automatic" variant of the work we do manually in 
// the "broker.cpp" application.

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

    zmq::proxy(frontend, backend);

    return 0;
}