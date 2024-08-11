#include "utils/logger.h"

#include <nlohmann/json.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace
{

LOG_CAT(log_cat, "main")

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using stream = websocket::stream<
	typename beast::tcp_stream::rebind_executor<
		typename net::use_awaitable_t<>::executor_with_default<net::any_io_executor>>::other>;

//------------------------------------------------------------------------------

// Echoes back all received WebSocket messages
net::awaitable<void>
do_session(stream ws)
{
	// Set suggested timeout settings for the websocket
	ws.set_option(
		websocket::stream_base::timeout::suggested(
			beast::role_type::server));

	// Set a decorator to change the Server of the handshake
	ws.set_option(websocket::stream_base::decorator(
		[](websocket::response_type &res)
		{
			res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-server-coro");
		}));

	// Accept the websocket handshake
	co_await ws.async_accept();

	for (;;)
		try {
			// This buffer will hold the incoming message
			beast::flat_buffer buffer;

			// TODO: send json !!!
			std::string json_string {};
			nlohmann::json json;
			json["age"] = 12;
			json["time"] = 10.112;
			json["name"] = "Dan";
			json.get_to(json_string);

			// Read a message
			co_await ws.async_read(buffer);

			// Echo the message back
			ws.text(ws.got_text());
			co_await ws.async_write(buffer.data());

		}
		catch (boost::system::system_error &se) {
			if (se.code() != websocket::error::closed)
				throw;

		}
}

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
net::awaitable<void>
do_listen(
	tcp::endpoint endpoint)
{

	// Open the acceptor
	auto acceptor = net::use_awaitable.as_default_on(tcp::acceptor(co_await net::this_coro::executor));
	acceptor.open(endpoint.protocol());

	// Allow address reuse
	acceptor.set_option(net::socket_base::reuse_address(true));

	// Bind to the server address
	acceptor.bind(endpoint);

	// Start listening for connections
	acceptor.listen(net::socket_base::max_listen_connections);

	for (;;)
		boost::asio::co_spawn(
			acceptor.get_executor(),
			do_session(stream(co_await acceptor.async_accept())),
			[](std::exception_ptr e)
			{
				try {
					std::rethrow_exception(e);
				}
				catch (std::exception &e) {
					log_info(log_cat, e.what());
				}
			});
}

}

int main(int argc, char* argv[])
{

	auto const address = net::ip::make_address("127.0.0.1");
	auto const port = static_cast<unsigned short>(std::atoi("80"));
	auto const threads = std::max<int>(1, 1);

	// The io_context is required for all I/O
	net::io_context ioc(threads);

	// Spawn a listening port
	boost::asio::co_spawn(
		ioc,
		do_listen(tcp::endpoint{address, port}),
		[](std::exception_ptr e)
		{
			if (e)
				try
				{
					std::rethrow_exception(e);
				}
				catch(std::exception & e)
				{
					std::cerr << "Error: " << e.what() << "\n";
				}
		});

	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for(auto i = threads - 1; i > 0; --i)
		v.emplace_back(
			[&ioc]
			{
				ioc.run();
			});
	ioc.run();

	return EXIT_SUCCESS;
}
