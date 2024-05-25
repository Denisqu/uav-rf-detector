#include "helloworld.grpc.pb.h"

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <agrpc/client_rpc.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/optional.hpp>

#include <QApplication>
#include <QQmlApplicationEngine>

namespace example
{
template <auto PrepareAsync>
using AwaitableClientRPC = boost::asio::use_awaitable_t<>::as_default_on_t<agrpc::ClientRPC<PrepareAsync>>;
}

#include <exception>

namespace example
{
// Using this as the completion token to functions like asio::co_spawn ensures that exceptions thrown by the coroutine
// are rethrown from grpc_context.run().
struct RethrowFirstArg
{
    template <class... T>
    void operator()(std::exception_ptr ep, T&&...)
    {
        if (ep)
        {
            std::rethrow_exception(ep);
        }
    }

    template <class... T>
    void operator()(T&&...)
    {
    }
};
}  // namespace example

int old_main(int argc, const char** argv) {
    const auto port = argc >= 2 ? argv[1] : "50051";
    const auto host = std::string("localhost:") + port;

    grpc::Status status;

    helloworld::Greeter::Stub stub{grpc::CreateChannel(host, grpc::InsecureChannelCredentials())};
    agrpc::GrpcContext grpc_context;


    boost::asio::co_spawn (
        grpc_context,
        [&]() -> boost::asio::awaitable<void>
        {
            using RPC = example::AwaitableClientRPC<&helloworld::Greeter::Stub::PrepareAsyncSayHello>;
            grpc::ClientContext client_context;
            helloworld::HelloRequest request;
            request.set_name("world");
            helloworld::HelloReply response;
            status = co_await RPC::request(grpc_context, stub, client_context, request, response);
            std::cout << status.ok() << " response: " << response.message() << std::endl;
        },
        example::RethrowFirstArg{});

    grpc_context.run();

    //abort_if_not(status.ok());
    return 0;
}

#include <iostream>
int main(int argc, char** argv)
{
    const auto test = boost::make_optional(12);
    std::cout << "test = " << *test;
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine appEngine(QUrl("qrc:/main.qml"));
    return app.exec();
}
