#ifndef DETECTION_SERVICE_H
#define DETECTION_SERVICE_H

#include "rpc_types.h"
#include "service_concept.h"

#include "libcuckoo/cuckoohash_map.hh"

namespace server { class AsyncGrpcServer; }

namespace service
{

class DetectionServiceHandler :
	private ServiceConceptValidator<DetectionServiceHandler>
{
public:
	using RPC = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;

	DetectionServiceHandler(server::AsyncGrpcServer& server);

public:
	void onTimeout();
	boost::asio::awaitable<void> operator() (service::DetectionServiceHandler::RPC& rpc);

private:
	libcuckoo::cuckoohash_map<std::string, grpc::ServerContext*> m_clients {};
	server::AsyncGrpcServer& m_server;
};

}

#endif //DETECTION_SERVICE_H
