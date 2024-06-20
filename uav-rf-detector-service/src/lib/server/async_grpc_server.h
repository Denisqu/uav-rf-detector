#ifndef ASYNCGRPCSERVER_H
#define ASYNCGRPCSERVER_H

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <agrpc/server_rpc.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <agrpc/asio_grpc.hpp>
#include <boost/asio/thread_pool.hpp>
#include <libcuckoo/cuckoohash_map.hh>

namespace server
{



class AsyncGrpcServer
{
public:
	explicit AsyncGrpcServer(std::string port = "50051");
	bool startListening();
	boost::asio::thread_pool& getThreadPool();
	agrpc::GrpcContext &getContext();


private:
	void registerServices(agrpc::GrpcContext& context, grpc::ServerBuilder& builder);

private:
	std::string m_port {};
	std::unique_ptr<grpc::Server> m_server = nullptr;
	std::unique_ptr<boost::asio::thread_pool> m_threadPool = nullptr;
	std::unique_ptr<agrpc::GrpcContext> m_context = nullptr;
	libcuckoo::cuckoohash_map<std::string, grpc::ServerContext*> m_clients {};
};

}

#endif //ASYNCGRPCSERVER_H
