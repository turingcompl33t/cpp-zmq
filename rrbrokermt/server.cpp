// server.cpp
// Refactor of request/reply pattern with intermediary broker
// in which the "server" application implements both the broker and 
// the worker nodes as distinct threads rather than separate processes.

#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include <zmq.hpp>

void worker(zmq::context_t* ctx, unsigned id)
{
    using namespace std::chrono_literals;

    zmq::socket_t socket{*ctx, ZMQ_REP};
    socket.connect("inproc://work-dealer");

    const std::string data{"World"};

    for (;;)
    {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);
        std::cout << "[" << id << "] ";
        std::cout << "Received " << request.to_string(); 
        std::cout << std::endl;

        // simulate work
        std::this_thread::sleep_for(1s);

        // construct a reply message
        zmq::message_t reply{data.cbegin(), data.cend()};

        // send the reply to the client
        socket.send(reply, zmq::send_flags::none);
    }
}

int main() 
{
    using namespace std::chrono_literals;

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    zmq::socket_t frontend{context, ZMQ_ROUTER};  // for REQ socket (client)
    zmq::socket_t backend{context, ZMQ_DEALER};   // for REP socket (workers)

    frontend.bind("tcp://*:5555");
    backend.bind("inproc://work-dealer");

    std::vector<std::thread> threads{};
    for (unsigned thread_id = 0; thread_id < 5; ++thread_id)
    {
        threads.emplace_back(worker, &context, thread_id);
    }

    zmq::proxy(frontend, backend);

    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return 0;
}