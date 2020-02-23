// source.cpp
// A simple example that pushes a single multi-part message.

#include <vector>
#include <string>

#include <zmq.hpp>

int main()
{
    zmq::context_t context{1};

    zmq::socket_t source{context, ZMQ_REQ};
    source.connect("tcp://127.0.0.1:5555");

    std::vector<std::string> fragments{};
    fragments.emplace_back("where ");
    fragments.emplace_back("is ");
    fragments.emplace_back("my ");
    fragments.emplace_back("mind?");

    for (auto const& f : fragments)
    {
        zmq::message_t msg{f.begin(), f.end()};
        source.send(msg, zmq::send_flags::sndmore);
    }

    zmq::message_t done{};
    source.send(done, zmq::send_flags::none);

    return 0;
}