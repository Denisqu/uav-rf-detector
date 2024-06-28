#include "async_grpc_server.h"
#include "lib/server/server_utils.h"
#include "lib/service/detection_service.h"
#include "lib/utils/logger.h"

#include <boost/asio/as_tuple.hpp>

#include <thread>
#include <functional>
#include "lib/general/app_mediator.h"

namespace {

LOG_CAT(log_cat, "server.async_grpc_server")

}

namespace server
{

AsyncGrpcServer::AsyncGrpcServer(AppMediator& app, std::string port)
	: m_port(std::move(port))
	, app(app)
	, m_threadPool(std::make_unique<boost::asio::thread_pool>(
		std::thread::hardware_concurrency() == 0 ? 2 : std::thread::hardware_concurrency())
	)
{
}

bool AsyncGrpcServer::startListening()
{
	auto builder = grpc::ServerBuilder();
	m_context = std::make_unique<agrpc::GrpcContext>(builder.AddCompletionQueue());
	registerServices(builder);
	const auto host = std::string("0.0.0.0:") + m_port;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());

	m_server = builder.BuildAndStart();
	log_info(log_cat, "Starting server on {}", host);
	return m_context->run();
}

template<typename Service, service::ServiceConcept Handler, typename... HandlerArgs>
void AsyncGrpcServer::registerService(grpc::ServerBuilder& builder, HandlerArgs... args)
{
	static auto service = Service();
	static auto handler = Handler(std::forward<HandlerArgs>(args)...);
	agrpc::register_awaitable_rpc_handler<service::DetectionServiceHandler::RPC>(
		*m_context, service, handler, server::RethrowFirstArg{}
    );
	builder.RegisterService(&service);
	log_info(log_cat, "Registered service: {}", handler.name());
}

void AsyncGrpcServer::registerServices(grpc::ServerBuilder& builder)
{
	registerService<rfdetector::DetectionService::AsyncService, service::DetectionServiceHandler>(
		builder, std::ref(*this)
	);
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