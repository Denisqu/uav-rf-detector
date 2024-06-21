#include "lib/server/async_grpc_server.h"

#include "lib/utils/logger.h"

namespace {

LOG_CAT(log_cat, "main")

}

int main(int argc, const char** argv)
{
	log_info(log_cat, "Start of application");

	//auto server = server::AsyncGrpcServer();
	//return server.startListening();
}