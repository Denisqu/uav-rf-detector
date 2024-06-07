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
	const auto host = std::string("0.0.0.0:") + m_port;

	helloworld::Greeter::AsyncService service;
	std::unique_ptr<grpc::Server> server;

	grpc::ServerBuilder builder;
	agrpc::GrpcContext grpc_context{builder.AddCompletionQueue()};
	using RPC = AwaitableServerRPC<&helloworld::Greeter::AsyncService::RequestSayHello>;
	agrpc::register_awaitable_rpc_handler<RPC>(
		grpc_context, service,
		[&](RPC &rpc, RPC::Request &request) -> boost::asio::awaitable<void>
		{
			helloworld::HelloReply response;
			response.set_message("Hello " + request.name());
			co_await rpc.finish(response, grpc::Status::OK);
			server->Shutdown();
		},
		server::RethrowFirstArg{});

	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	server = builder.BuildAndStart();

	return grpc_context.run();
}

}