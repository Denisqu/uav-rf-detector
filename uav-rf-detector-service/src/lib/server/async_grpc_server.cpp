#include "async_grpc_server.h"
#include "lib/server/server_utils.h"
#include "helloworld.grpc.pb.h"
#include "rfdetector.grpc.pb.h"

namespace server
{

AsyncGrpcServer::AsyncGrpcServer(std::string port)
	: m_port(std::move(port))
{}

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
	// TODO: убрать после того как минимально настрою rfdetector::DetectionService
	static auto greeterService = helloworld::Greeter::AsyncService();
	using RPC = AwaitableServerRPC<&helloworld::Greeter::AsyncService::RequestSayHello>;
	agrpc::register_awaitable_rpc_handler<RPC>(
		context, greeterService,
		[this](RPC &rpc, RPC::Request &request) -> boost::asio::awaitable<void>
		{
			helloworld::HelloReply response;
			response.set_message("Hello " + request.name());
			co_await rpc.finish(response, grpc::Status::OK);
			m_server->Shutdown();
		},
		server::RethrowFirstArg{});
	builder.RegisterService(&greeterService);

	static auto detectionService = rfdetector::DetectionService::AsyncService();
	using RPC2 = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;
	agrpc::register_awaitable_rpc_handler<RPC2>(
		context, detectionService,
		[this](RPC2 &rpc, RPC2::Request &request) -> boost::asio::awaitable<void>
		{
			rfdetector::ResponseStream response;
			if (request.has_keepalive()) {
				rfdetector::Detection detection;
				// Populate the detection message as needed
				response.set_allocated_detection(&detection);
			}
			co_await rpc.write(response);
			// Continue reading the next request
			while (co_await rpc.read(request)) {
				if (request.has_keepalive()) {
					rfdetector::Detection detection;
					// Populate the detection message as needed
					response.set_allocated_detection(&detection);
				}
				co_await rpc.write(response);
			}
			co_await rpc.finish(grpc::Status::OK);
		},
		server::RethrowFirstArg{});
	builder.RegisterService(&detectionService);
}

}