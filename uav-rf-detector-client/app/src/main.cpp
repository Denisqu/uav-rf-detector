#include "helloworld.grpc.pb.h"
#include "rfdetector.grpc.pb.h"

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <agrpc/alarm.hpp>
#include <agrpc/client_rpc.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/parallel_group.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <agrpc/client_rpc.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/optional.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string generate_uuid() {
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	return to_string(uuid);
}

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

// begin-snippet: client-rpc-bidirectional-streaming
// ---------------------------------------------------
// A bidirectional-streaming request that simply sends the response from the server back to it.
// ---------------------------------------------------
// end-snippet
boost::asio::awaitable<void> make_bidirectional_streaming_request(agrpc::GrpcContext& grpc_context, rfdetector::DetectionService::Stub& stub)
{
	using RPC2 = example::AwaitableClientRPC<&rfdetector::DetectionService::Stub::PrepareAsyncMainStream>;

	RPC2 rpc {grpc_context};
	rpc.context().set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));
	std::string uuid = generate_uuid();
	rpc.context().AddMetadata("uuid", uuid);
	if (!co_await rpc.start(stub))
	{
		// Channel is either permanently broken or transiently broken but with the fail-fast option.
		co_return;
	}

	// Perform a request/response ping-pong.
	rfdetector::RequestStream request;

	// TODO: убрать аллокацию
	auto keepAlive = new rfdetector::KeepAlive();
	request.mutable_keepalive()->CopyFrom(*keepAlive);
	rfdetector::ResponseStream response;

	// Reads and writes can be performed simultaneously.
	using namespace boost::asio::experimental::awaitable_operators;
	auto [read_ok, write_ok] = co_await (rpc.read(response) && rpc.write(request));

	int count{};
	while (read_ok && write_ok && count < 10)
	{
		std::cout << "has detection: " << response.has_detection()
				  << "detection:"      << response.detection().DebugString();
		++count;
		std::tie(read_ok, write_ok) = co_await (rpc.read(response) && rpc.write(request));
	}

	// Finish will automatically signal that the client is done writing. Optionally call rpc.writes_done() to explicitly
	// signal it earlier.
	const grpc::Status status = co_await rpc.finish();

	if (!status.ok()) {
		abort();
	}
}

int new_old_main(int argc, const char** argv) {
	const auto port = argc >= 2 ? argv[1] : "50051";
	const auto host = std::string("localhost:") + port;

	const auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	auto stub = rfdetector::DetectionService::Stub { channel };
	agrpc::GrpcContext grpc_context;

	boost::asio::co_spawn(
		grpc_context,
		[&]() -> boost::asio::awaitable<void>
		{
			co_await make_bidirectional_streaming_request(grpc_context, stub);
		},
		example::RethrowFirstArg{});

	return grpc_context.run();
}

#include <iostream>
int main(int argc, const char **argv)
{
	new_old_main(argc, argv);

    QGuiApplication app(argc, const_cast<char**>(argv));

	// TODO: убрать использование шрифта из билд директории
    qint32 fontId = QFontDatabase::addApplicationFont("./Roboto-Regular.ttf");
    QStringList fontList = QFontDatabase::applicationFontFamilies(fontId);

    QString family = fontList.first();
    QGuiApplication::setFont(QFont(family));

    QQmlApplicationEngine appEngine(QUrl("qrc:/qt/qml/app/assets/qml/main.qml"));
    return app.exec();
}
