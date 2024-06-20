#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include "rfdetector.grpc.pb.h"
#include "boost/asio/use_awaitable.hpp"
#include <agrpc/asio_grpc.hpp>

namespace service
{

template<auto RequestRPC>
using AwaitableServerRPC = boost::asio::use_awaitable_t<>::as_default_on_t<agrpc::ServerRPC<RequestRPC>>;

}

#endif //RPC_TYPES_H
