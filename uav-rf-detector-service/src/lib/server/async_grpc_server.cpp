#include "async_grpc_server.h"
#include "lib/server/server_utils.h"
#include "lib/service/detection_service.h"

#include <boost/asio/as_tuple.hpp>
#include <thread>

namespace server
{

AsyncGrpcServer::AsyncGrpcServer(std::string port)
	: m_port(std::move(port))
	, m_threadPool(std::make_unique<boost::asio::thread_pool>(
		std::thread::hardware_concurrency() == 0 ? 2 : std::thread::hardware_concurrency())
	)
{
}

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
	static auto detectionServiceHandler = service::DetectionServiceHandler(*this);
	agrpc::register_awaitable_rpc_handler<service::DetectionServiceHandler::RPC>(
		context, detectionService,detectionServiceHandler,
		server::RethrowFirstArg{});
	builder.RegisterService(&detectionService);

}

boost::asio::thread_pool& AsyncGrpcServer::getThreadPool()
{
	return *m_threadPool;
}

}