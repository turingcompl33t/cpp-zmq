// ventilator.cpp
// Parallel task ventilator;
// basic demonstration of a divide-and-conquer pattern.

#include <thread>
#include <chrono>
#include <random>
#include <iostream>

#include <zmq.hpp>

int main()
{
    using namespace std::chrono_literals;

    // prepare a random number generator
    // and uniform int distribution
    std::random_device dev{};
    std::mt19937 eng{dev()};
    std::uniform_int_distribution<> dist{1, 100};

    zmq::context_t context{1};

    // socket on which messages will be sent
    zmq::socket_t sender{context, ZMQ_PUSH};
    sender.bind("tcp://*:5556");

    // socket to signal the sink 
    zmq::socket_t sink{context, ZMQ_PUSH};
    sink.connect("tcp://localhost:5557");

    std::cout << "Press ENTER when workers ready\n";
    std::cin.get();
    std::cout << "Sending tasks to workers...\n";
    
    // inform sink it is game time
    zmq::message_t message{2};
    memcpy(message.data(), "0", 1);
    sink.send(message, zmq::send_flags::none);

    // push out 100 tasks
    int total_msec = 0;
    for (auto task_num = 0; task_num < 100; ++task_num)
    {
        int workload = dist(eng);
        total_msec += workload;

        message.rebuild(10);
        sprintf_s(static_cast<char*>(message.data()), 10, "%d", workload);
        sender.send(message, zmq::send_flags::none);
    }

    std::cout << "Total expected cost: " << total_msec << '\n';

    std::this_thread::sleep_for(2s);

    return 0;
}