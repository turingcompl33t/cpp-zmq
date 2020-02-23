// sink.cpp
// Upgraded implementation of parallel task sink
// to support worker shutdown upon task exhaustion.

#include <thread>
#include <chrono>
#include <iostream>

#include <zmq.hpp>

int main()
{
    zmq::context_t context{1};
    
    // socket on which to recv completions
    zmq::socket_t recver{context, ZMQ_PULL};
    recver.bind("tcp://*:5556");

    // socket on which to publish "all done" signal
    zmq::socket_t publisher{context, ZMQ_PUB};
    publisher.bind("tcp://*:5557");

    // wait for batch start
    zmq::message_t message{};
    recver.recv(message, zmq::recv_flags::none);

    auto start = std::chrono::high_resolution_clock::now();

    // process 100 confirmations
    for (auto task_num = 0; task_num < 100; ++task_num)
    {
        recver.recv(message, zmq::recv_flags::none);
        if ((task_num / 10) * 10 == task_num)
        {
            std::cout << ":" << std::flush;
        }
        else
        {
            std::cout << "." << std::flush;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    std::cout << "\nTotal elapsed time: " << ms.count() << "ms\n";

    // signal workers to shut down
    message.rebuild();
    publisher.send(message, zmq::send_flags::none);

    return 0;
}