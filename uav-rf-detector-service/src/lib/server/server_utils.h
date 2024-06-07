#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <exception>

namespace server {

// Using this as the completion token to functions like asio::co_spawn ensures that exceptions thrown by the coroutine
// are rethrown from grpc_context.run().
struct RethrowFirstArg
{
	template <class... T>
	void operator()(std::exception_ptr ep, T&&...)
	{
		if (ep)
		{
			std::rethrow_exception(ep);
		}
	}

	template <class... T>
	void operator()(T&&...)
	{
	}
};

}

#endif //SERVER_UTILS_H
