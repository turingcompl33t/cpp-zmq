// worker.cpp
// Parallel task worker;
// demonstrates basic divide-and-conquer pattern.

#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

#include <zmq.hpp>

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

    // process tasks forever
    for (;;)
    {
        zmq::message_t message;
        int workload;

        // recv the message 
        recver.recv(message, zmq::recv_flags::none);
        std::string data{static_cast<char*>(message.data()), message.size()};

        // parse the workload
        std::istringstream iss{data};
        iss >> workload;

        // do some work
        std::this_thread::sleep_for(std::chrono::milliseconds{workload});

        message.rebuild();
        sender.send(message, zmq::send_flags::none);

        std::cout << "." << std::flush;
    }

    return 0;
}