// program.cpp
// Demonstration of basic inter-thread signalling 
// with PAIR sockets and ZMQ message passing.

#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <iostream>

#include <zmq.hpp>

constexpr auto N_THREADS = 5;

using sockets_t = std::vector<zmq::socket_t>;
using threads_t = std::vector<std::thread>;

void worker(zmq::context_t* ctx, unsigned id)
{
    std::string const bind_base{"inproc://worker-"};
    auto const bind_addr = bind_base + std::to_string(id);

    zmq::socket_t socket{*ctx, ZMQ_PAIR};
    socket.connect(bind_addr);

    std::cout << "." << std::flush;

    // wait for signal
    zmq::message_t message{};
    socket.recv(message, zmq::recv_flags::none);

    return;
}

int main()
{
    using namespace std::chrono_literals;

    zmq::context_t context{1};

    sockets_t sockets{};
    threads_t threads{};

    std::string const bind_base{"inproc://worker-"};

    // create a socket for each thread
    for (unsigned i = 0; i < N_THREADS; ++i)
    {
        auto const bind_addr = bind_base + std::to_string(i);
        
        zmq::socket_t socket{context, ZMQ_PAIR};
        socket.bind(bind_addr);
        
        sockets.push_back(std::move(socket));
    }

    // start the worker threads
    for (unsigned id = 0; id < N_THREADS; ++id)
    {
        threads.emplace_back(worker, &context, id);
    }

    // wait for a bit
    std::this_thread::sleep_for(2s);

    // signal all threads to exit
    for (auto& s : sockets)
    {
        zmq::message_t message{};
        s.send(message, zmq::send_flags::none);
    }

    // ensure all threads exit properly
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return 0;
}