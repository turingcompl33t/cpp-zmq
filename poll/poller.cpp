// poller.cpp
// Basic demonstration of handling multiple sockets via polling;
// functions as both a generic subscriber and a parallel task worker.

#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

#include <zmq.hpp>

int main()
{
    using namespace std::chrono_literals;

    zmq::context_t context{1};

    // socket to recv publisher messages on
    zmq::socket_t subscriber{context, ZMQ_SUB};
    subscriber.connect("tcp://localhost:5555");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    // socket to recv tasks on
    zmq::socket_t recver{context, ZMQ_PULL};
    recver.connect("tcp://localhost:5556");

    // socket to send completions on
    zmq::socket_t sender{context, ZMQ_PUSH};
    sender.connect("tcp://localhost:5557");

    zmq::pollitem_t items[] = {
       { static_cast<void*>(recver), 0, ZMQ_POLLIN, 0 },
       { static_cast<void*>(subscriber), 0, ZMQ_POLLIN, 0 }
    };

    // process tasks forever
    for (;;)
    {
        zmq::message_t message;
        int workload;

        zmq::poll(&items[0], 2, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            // recv the message 
            recver.recv(message, zmq::recv_flags::none);
            auto data = message.to_string();

            // parse the workload
            std::istringstream iss{data};
            iss >> workload;

            // do some work
            std::this_thread::sleep_for(std::chrono::milliseconds{workload});

            message.rebuild();
            sender.send(message, zmq::send_flags::none);
        }

        if (items[1].revents & ZMQ_POLLIN)
        {
            subscriber.recv(message, zmq::recv_flags::none);
            auto data = message.to_string();

            std::cout << "[RECV] " << data << std::endl;
        }
    }

    return 0;
}