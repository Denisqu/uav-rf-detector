#ifndef DETECTION_SERVICE_H
#define DETECTION_SERVICE_H

#include "rpc_types.h"
#include "service_concept.h"

#include "libcuckoo/cuckoohash_map.hh"
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/thread_pool.hpp>

namespace server { class AsyncGrpcServer; }

namespace service
{

class DetectionServiceHandler :
	private ServiceConceptValidator<DetectionServiceHandler>
{
public:
	using RPC = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;

private:
	using Channel = boost::asio::experimental::channel<void(boost::system::error_code, rfdetector::RequestStream)>;

public:
	DetectionServiceHandler(server::AsyncGrpcServer& server);
	void onTimeout();
	boost::asio::awaitable<void> operator() (service::DetectionServiceHandler::RPC& rpc);

private:
	boost::asio::awaitable<bool> writer(service::DetectionServiceHandler::RPC& rpc, Channel& channel, boost::asio::thread_pool& thread_pool);
	boost::asio::awaitable<void> reader(service::DetectionServiceHandler::RPC& rpc, Channel& channel);

public:
	bool hearbeatReceived = false;

private:
	libcuckoo::cuckoohash_map<std::string, grpc::ServerContext*> m_clients {};
	server::AsyncGrpcServer& m_server;
};

}

#endif //DETECTION_SERVICE_H
