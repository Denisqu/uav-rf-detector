#ifndef SERVICE_CONCEPT_H
#define SERVICE_CONCEPT_H

#include "rpc_types.h"

#include <iostream>
#include <string>
#include <type_traits>

#include <boost/asio/awaitable.hpp>

namespace service
{

template<typename Service>
concept ServiceConcept = requires(Service service, Service::RPC& rpc, int k) {
	{ service.onTimeout() } -> std::same_as<void>;
    { service.operator()(rpc) } -> std::same_as<boost::asio::awaitable<void>>;
};

/*!
 * @brief Use this function in constructor of Service to
 * ensure that service fullfils concept requirements in compile-time
 * @tparam Service
 * @return
 */
template<typename Service>
constexpr void validateServiceConcept()
{
	static_assert(ServiceConcept<Service>, "Service doesn't fullfil service concept!");
}

template<typename Service>
class ServiceConceptValidator
{
public:
	ServiceConceptValidator() {
		validateServiceConcept<Service>();
	}
};

}
#endif //SERVICE_CONCEPT_H
