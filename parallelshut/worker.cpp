// worker.cpp
// Upgraded implementation of parallel task worker
// to support worker shutdown upon task exhaustion.

#include <chrono>
#include <thread>
#include <vector>
#include <sstream>
#include <iostream>

#include <zmq.hpp>

using poll_items_t = std::vector<zmq::pollitem_t>;

int main()
{
    using namespace std::chrono_literals;

    zmq::context_t context{1};

    // socket to recv tasks on
    zmq::socket_t recver{context, ZMQ_PULL};
    recver.connect("tcp://localhost:5555");

    // socket to send completions on
    zmq::socket_t sender{context, ZMQ_PUSH};
    sender.connect("tcp://localhost:5556");

    // socket on which to subscribe to kill signal
    zmq::socket_t subscriber{context, ZMQ_SUB};
    subscriber.connect("tcp://localhost:5557");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    poll_items_t items{};
    items.push_back({static_cast<void*>(recver), 0, ZMQ_POLLIN, 0});
    items.push_back({static_cast<void*>(subscriber), 0, ZMQ_POLLIN, 0});

    bool shutdown = false;

    // process tasks forever
    while (!shutdown)
    {
        zmq::message_t message;
        int workload;

        zmq::poll(items, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            // recv a work message

            recver.recv(message, zmq::recv_flags::none);
            std::string data{static_cast<char*>(message.data()), message.size()};

            std::istringstream iss{data};
            iss >> workload;

            // do some work
            std::this_thread::sleep_for(std::chrono::milliseconds{workload});

            message.rebuild();
            sender.send(message, zmq::send_flags::none);

            std::cout << "." << std::flush;
        }

        if (items[1].revents & ZMQ_POLLIN)
        {
            std::cout << "\nSHUTDOWN" << std::endl;
            shutdown = true;
        }
    }

    return 0;
}