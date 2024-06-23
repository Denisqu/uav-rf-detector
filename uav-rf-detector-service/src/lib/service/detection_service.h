#ifndef DETECTION_SERVICE_H
#define DETECTION_SERVICE_H

#include "rpc_types.h"
#include "service_concept.h"

#include "libcuckoo/cuckoohash_map.hh"
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/steady_timer.hpp>

namespace server { class AsyncGrpcServer; }

namespace service
{

class DetectionServiceHandler :
	private ServiceConceptValidator<DetectionServiceHandler>
{
public:
	using RPC = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;

private:
	using RequestChannel = boost::asio::experimental::channel<void(boost::system::error_code, rfdetector::RequestStream)>;
	using MasterChannel = boost::asio::experimental::channel<void(boost::system::error_code, rfdetector::RequestStream)>;

public:
	DetectionServiceHandler(server::AsyncGrpcServer& server);
	void onKeepAliveTimeout(const boost::system::error_code& error, const std::string& clientUuid);
	boost::asio::awaitable<void> operator() (service::DetectionServiceHandler::RPC& rpc);
	const std::string& name();

private:
	boost::asio::awaitable<void> reader(service::DetectionServiceHandler::RPC& rpc, RequestChannel& channel);
	boost::asio::awaitable<bool> slaveWriter(service::DetectionServiceHandler::RPC& rpc, RequestChannel& channel, boost::asio::thread_pool& thread_pool);
	boost::asio::awaitable<bool> masterWriter(service::DetectionServiceHandler::RPC& rpc, MasterChannel& channel, boost::asio::thread_pool& thread_pool);
	void onDetectionFound();

private:
	libcuckoo::cuckoohash_map<std::string, grpc::ServerContext*> m_clients {}; // TODO: возможно сюда нужно закинуть шаред_птр
	libcuckoo::cuckoohash_map<std::string, std::shared_ptr<boost::asio::steady_timer>> m_keepAliveTimers {};
	server::AsyncGrpcServer& m_server;

	std::string m_name = "DetectionService";
};

}

#endif //DETECTION_SERVICE_H
