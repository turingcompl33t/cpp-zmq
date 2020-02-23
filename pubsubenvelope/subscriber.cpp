// subscriber.cpp
// Simple pub/sub pattern subscriber demonstrating use of envelopes.

#include <iostream>

#include <zmq.hpp>

int main()
{
    zmq::context_t context{1};

    zmq::socket_t subscriber{context, ZMQ_SUB};
    subscriber.connect("tcp://127.0.0.1:5555");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "B", 1);

    for (;;)
    {
        zmq::message_t tag{};
        zmq::message_t content{};

        subscriber.recv(tag, zmq::recv_flags::none);
        if (!tag.more())
        {
            std::cout << "Something went wrong!\n";
            break;
        }

        subscriber.recv(content, zmq::recv_flags::none);
        if (content.more())
        {
            std::cout << "Something went wrong!\n";
            break;
        }

        std::cout << "Tag: " << tag.to_string();
        std::cout << " Content: " << content.to_string();
        std::cout << std::endl;
    }
}