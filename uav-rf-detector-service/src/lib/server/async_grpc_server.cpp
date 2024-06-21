#include "async_grpc_server.h"
#include "lib/server/server_utils.h"
#include "lib/service/detection_service.h"
#include "lib/utils/logger.h"

#include <boost/asio/as_tuple.hpp>
#include <thread>

namespace {

LOG_CAT(log_cat, "server.async_grpc_server")

}

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
	m_context = std::make_unique<agrpc::GrpcContext>(builder.AddCompletionQueue());
	registerServices(*m_context, builder);
	const auto host = std::string("0.0.0.0:") + m_port;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());

	m_server = builder.BuildAndStart();
	log_info(log_cat, "Starting server on {}", host);
	return m_context->run();
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

agrpc::GrpcContext& AsyncGrpcServer::getContext()
{
	return *m_context;
}

}