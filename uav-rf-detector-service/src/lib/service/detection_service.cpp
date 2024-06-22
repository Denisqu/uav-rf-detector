#include "detection_service.h"
#include "lib/server/async_grpc_server.h"
#include "lib/utils/logger.h"

#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio.hpp>

#include <iostream>

namespace {

LOG_CAT(log_cat, "service.detection_service");

constexpr auto KEEPALIVE_TIMEOUT_TIME() {
	return std::chrono::milliseconds(600);
}

}

namespace service {

DetectionServiceHandler::DetectionServiceHandler(server::AsyncGrpcServer& server)
	: m_server(server) { }

boost::asio::awaitable<void> DetectionServiceHandler::operator()(DetectionServiceHandler::RPC &rpc)
{
	if (!rpc.context().client_metadata().contains("uuid")) {
		rpc.context().TryCancel();
		log_info(log_cat, "client doesn't have UUID metadata, aborting connection");
		co_return;
	}

	const std::string clientUuid = std::string(rpc.context().client_metadata().find("uuid")->second.data(),
											   rpc.context().client_metadata().find("uuid")->second.size());
	m_clients.insert(clientUuid,&rpc.context());


	auto keepAliveTimer = std::make_shared<boost::asio::steady_timer>(m_server.getContext(),
													std::chrono::steady_clock::now() + KEEPALIVE_TIMEOUT_TIME());
	keepAliveTimer->async_wait([this, clientUuid](const auto& error) {
		onKeepAliveTimeout(error, clientUuid);
	});
	m_keepAliveTimers.insert(clientUuid, std::move(keepAliveTimer));


	log_info(log_cat, "rpc started, client UUID: {}",
			 rpc.context().client_metadata().find("uuid")->second);

	// Maximum number of requests that are buffered by the channel to enable backpressure.
	static constexpr auto MAX_BUFFER_SIZE = 64;

	SlaveChannel channel{co_await boost::asio::this_coro::executor, MAX_BUFFER_SIZE};

	using namespace boost::asio::experimental::awaitable_operators;
	const auto ok = co_await (reader(rpc, channel) && slaveWriter(rpc, channel, m_server.getThreadPool()));

	if (!ok)
	{
		// Client has disconnected or server is shutting down.
		log_info(log_cat, "client disconnetcted {}",
				 rpc.context().client_metadata().find("uuid")->second);
		co_return;
	}

	log_info(log_cat, "finish of service for client: {}", rpc.context().client_metadata().find("uuid")->second);
	co_await rpc.finish(grpc::Status::OK);
}

// This function will read one requests from the client at a time. Note that gRPC only allows calling agrpc::read after
// a previous read has completed.
boost::asio::awaitable<void> DetectionServiceHandler::reader(service::DetectionServiceHandler::RPC& rpc, SlaveChannel& channel)
{
	while (true)
	{
		rfdetector::RequestStream request;
		if (!co_await rpc.read(request))
		{
			// Client is done writing.
			break;
		}

		log_info(log_cat, "Request received: {}", request.DebugString());

		if (request.has_keepalive()) {
			auto clientUuid = std::string(rpc.context().client_metadata().find("uuid")->second.data(),
										  rpc.context().client_metadata().find("uuid")->second.size());
			log_info(log_cat, "keepalive received from client: {} - {}", rpc.context().peer(), clientUuid);
			auto pKeepAliveTimer = m_keepAliveTimers.find(clientUuid);
			pKeepAliveTimer->expires_after(KEEPALIVE_TIMEOUT_TIME());
			pKeepAliveTimer->async_wait([this, clientUuid](const auto& error) {
				onKeepAliveTimeout(error, clientUuid);
			});
		}

		// Send request to slaveWriter. The `max_buffer_size` of the channel acts as backpressure.
		(void)co_await channel.async_send(boost::system::error_code{}, std::move(request),
										  boost::asio::as_tuple(boost::asio::use_awaitable));
	}
	// Signal the slaveWriter to complete.
	channel.close();
}

// The slaveWriter will pick up reads from the reader through the channel and switch to the thread_pool to compute their
// response.
boost::asio::awaitable<bool> DetectionServiceHandler::slaveWriter(service::DetectionServiceHandler::RPC& rpc, SlaveChannel& channel, boost::asio::thread_pool& thread_pool)
{
	bool ok{true};
	while (ok)
	{
		const auto [ec, request] = co_await channel.async_receive(
			boost::asio::as_tuple(boost::asio::use_awaitable));
		if (ec)
		{
			// SlaveChannel got closed by the reader.
			break;
		}
		// switch context to the thread_pool to compute the response.
		co_await boost::asio::post(boost::asio::bind_executor(thread_pool, boost::asio::use_awaitable));

		// Compute the response.
		rfdetector::ResponseStream response;



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

void DetectionServiceHandler::onKeepAliveTimeout(const boost::system::error_code& error, const std::string& clientUuid)
{
	if (error) {
		return;
	}
	try {
		auto* clientContext = m_clients.find(clientUuid);
		log_info(log_cat, "no heartbeat was received from client, aborting connection!");
		clientContext->TryCancel();
	} catch (...) {};
}

const std::string &DetectionServiceHandler::name()
{
	return m_name;
}

}