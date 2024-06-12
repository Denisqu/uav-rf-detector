#include "async_grpc_server.h"
#include "lib/server/server_utils.h"

#include "helloworld.grpc.pb.h"
#include "rfdetector.grpc.pb.h"

#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/as_tuple.hpp>
#include <thread>

namespace server
{

AsyncGrpcServer::AsyncGrpcServer(std::string port)
	: m_port(std::move(port))
	, m_threadPool(std::make_unique<boost::asio::thread_pool>(
		std::thread::hardware_concurrency() == 0 ? 2 : std::thread::hardware_concurrency())
	)
{}

using Channel = boost::asio::experimental::channel<void(boost::system::error_code, rfdetector::RequestStream)>;
using RPC2 = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;

// This function will read one requests from the client at a time. Note that gRPC only allows calling agrpc::read after
// a previous read has completed.
boost::asio::awaitable<void> reader(RPC2& rpc, Channel& channel)
{


	while (true)
	{
		rfdetector::RequestStream request;
		if (!co_await rpc.read(request))
		{
			// Client is done writing.
			break;
		}
		// Send request to writer. The `max_buffer_size` of the channel acts as backpressure.
		(void)co_await channel.async_send(boost::system::error_code{}, std::move(request),
										  boost::asio::as_tuple(boost::asio::use_awaitable));
	}
	// Signal the writer to complete.
	channel.close();
}

// The writer will pick up reads from the reader through the channel and switch to the thread_pool to compute their
// response.
boost::asio::awaitable<bool> writer(RPC2& rpc, Channel& channel, boost::asio::thread_pool& thread_pool)
{
	bool ok{true};
	while (ok)
	{
		const auto [ec, request] = co_await channel.async_receive(
			boost::asio::as_tuple(boost::asio::use_awaitable));
		if (ec)
		{
			// Channel got closed by the reader.
			break;
		}
		// switch context to the thread_pool to compute the response.
		co_await boost::asio::post(boost::asio::bind_executor(thread_pool, boost::asio::use_awaitable));

		// Compute the response.
		rfdetector::ResponseStream response;

		if (request.has_keepalive()) {
			std::cout << "keepalive received from client: " << rpc.context().peer();
		}

		// TODO: убрать аллокацию
		/*
		auto detection = new rfdetector::Detection {};
		detection->set_protocol("test");
		detection->set_carrier(2400);
		detection->set_timestamp(123456);
		response.set_allocated_detection(detection);
		*/

		// rpc.write() is thread-safe so we can interact with it from the thread_pool.
		ok = co_await rpc.write(response);
		// Now we are back on the main thread.
	}
	co_return ok;
}



// begin-snippet: server-side-helloworld
// ---------------------------------------------------
// Server-side hello world which handles exactly one request from the client before shutting down.
// ---------------------------------------------------
// end-snippet
bool AsyncGrpcServer::startListening()
{
	auto builder = grpc::ServerBuilder();
	auto grpc_context = agrpc::GrpcContext(builder.AddCompletionQueue());
	registerServices(grpc_context, builder);
	const auto host = std::string("0.0.0.0:") + m_port;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());

	m_server = builder.BuildAndStart();
	return grpc_context.run();
}

void AsyncGrpcServer::registerServices(agrpc::GrpcContext &context, grpc::ServerBuilder& builder)
{
	static auto detectionService = rfdetector::DetectionService::AsyncService();

	agrpc::register_awaitable_rpc_handler<RPC2>(
		context, detectionService,
		[this](RPC2 &rpc) -> boost::asio::awaitable<void>
		{
			if (!rpc.context().client_metadata().contains("uuid")) {
				rpc.context().TryCancel();
				std::cout << "client doesn't have UUID metadata, aborting connection" << std::endl;
				co_return;
			}

			std::cout << "rpc started, client UUID: " << rpc.context().client_metadata().find("uuid")->second; << std::cout;

			// Maximum number of requests that are buffered by the channel to enable backpressure.
			static constexpr auto MAX_BUFFER_SIZE = 2;

			Channel channel{co_await boost::asio::this_coro::executor, MAX_BUFFER_SIZE};

			using namespace boost::asio::experimental::awaitable_operators;

			const auto ok = co_await (reader(rpc, channel) && writer(rpc, channel, *m_threadPool));

			if (!ok)
			{
				// Client has disconnected or server is shutting down.
				co_return;
			}

			co_await rpc.finish(grpc::Status::OK);
		},
		server::RethrowFirstArg{});
	builder.RegisterService(&detectionService);

}

}