#include "lib/server/async_grpc_server.h"

int main(int argc, const char** argv)
{
	auto server = server::AsyncGrpcServer();
	return server.startListening();
}