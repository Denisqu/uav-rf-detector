#include "async_grpc_server.h"
#include "lib/server/server_utils.h"
#include "helloworld.grpc.pb.h"

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
}

}