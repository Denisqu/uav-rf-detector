#include "detection_service.h"
#include "lib/server/async_grpc_server.h"

#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio.hpp>

#include <iostream>

namespace {




}

namespace service {

DetectionServiceHandler::DetectionServiceHandler(server::AsyncGrpcServer& server)
	: m_server(server) { }

boost::asio::awaitable<void> DetectionServiceHandler::operator()(DetectionServiceHandler::RPC &rpc)
{
	if (!rpc.context().client_metadata().contains("uuid")) {
		rpc.context().TryCancel();
		std::cout << "client doesn't have UUID metadata, aborting connection" << std::endl;
		co_return;
	}

	const std::string clientUuid = rpc.context().client_metadata().find("uuid")->second.data();
	m_clients.insert(clientUuid,
					 &rpc.context());

	boost::asio::steady_timer new_timer(m_server.getContext(),
										std::chrono::steady_clock::now() + std::chrono::seconds(3));
	new_timer.async_wait([this, clientUuid](const auto& error) {
		if (error) {
			return;
		}
		try {
			auto* clientContext = m_clients.find(clientUuid);

			if (!this->hearbeatReceived) {
				clientContext->TryCancel();
				std::cout << "no heartbeat was received from client, aborting connection!" << std::endl;
			}
		} catch (...) {};
	});


	std::cout << "rpc started, client UUID: "
			  << rpc.context().client_metadata().find("uuid")->second
			  << std::endl;

	// Maximum number of requests that are buffered by the channel to enable backpressure.
	static constexpr auto MAX_BUFFER_SIZE = 64;

	Channel channel{co_await boost::asio::this_coro::executor, MAX_BUFFER_SIZE};

	using namespace boost::asio::experimental::awaitable_operators;

	const auto ok = co_await (reader(rpc, channel) && writer(rpc, channel, m_server.getThreadPool()));

	if (!ok)
	{
		// Client has disconnected or server is shutting down.
		co_return;
	}

	co_await rpc.finish(grpc::Status::OK);
}

// This function will read one requests from the client at a time. Note that gRPC only allows calling agrpc::read after
// a previous read has completed.
boost::asio::awaitable<void> DetectionServiceHandler::reader(service::DetectionServiceHandler::RPC& rpc, Channel& channel)
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
boost::asio::awaitable<bool> DetectionServiceHandler::writer(service::DetectionServiceHandler::RPC& rpc, Channel& channel, boost::asio::thread_pool& thread_pool)
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

		// TODO: проверять keepalive не во writer'е
		if (request.has_keepalive()) {
			std::cout << "keepalive received from client: " << rpc.context().peer();
			hearbeatReceived = true;
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

void DetectionServiceHandler::onTimeout()
{
	std::cout << "timeout happened!";
}

}