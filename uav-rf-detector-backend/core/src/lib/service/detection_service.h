#ifndef DETECTION_SERVICE_H
#define DETECTION_SERVICE_H

#include "rpc_types.h"
#include "service_concept.h"

#include "libcuckoo/cuckoohash_map.hh"
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/steady_timer.hpp>

#include <utility>

namespace server { class AsyncGrpcServer; }

namespace service
{

class DetectionServiceHandler :
	private ServiceConceptValidator<DetectionServiceHandler>
{
public:
	using RPC = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;
	using ClientDataPtr = std::shared_ptr<grpc::ServerContext*

private:
	using SlaveChannel = boost::asio::experimental::channel<void(boost::system::error_code, rfdetector::RequestStream)>;

public:
	DetectionServiceHandler(server::AsyncGrpcServer& server);
	void onKeepAliveTimeout(const boost::system::error_code& error, const std::string& clientUuid);
	boost::asio::awaitable<void> operator() (service::DetectionServiceHandler::RPC& rpc);
	const std::string& name();

private:
	boost::asio::awaitable<void> reader(service::DetectionServiceHandler::RPC& rpc, SlaveChannel& channel);

	/*!
	 * @brief Отправляет ответы на реквесты, присланные пользователем
	 */
	boost::asio::awaitable<bool> slaveWriter(service::DetectionServiceHandler::RPC& rpc, SlaveChannel& channel, boost::asio::thread_pool& thread_pool);

	/*!
	 * @brief Отправляет потоковые данные пользователям без инициации запроса клиентом
	 * (отправка обнаружений, потоковых данных радиоэфира и т.д)
	 */
	//boost::asio::awaitable<bool> masterWriter(service::DetectionServiceHandler::RPC& rpc, MasterChannel& channel, boost::asio::thread_pool& thread_pool);

	boost::asio::awaitable<bool> onDetectionFound(rfdetector::Detection detection);

private:
	libcuckoo::cuckoohash_map<std::string, std::shared_ptr<RPC>> m_clients {};
	libcuckoo::cuckoohash_map<std::string, std::shared_ptr<boost::asio::steady_timer>> m_keepAliveTimers {};
	server::AsyncGrpcServer& m_server;

	const std::string m_name = "DetectionService";
};

}

#endif //DETECTION_SERVICE_H
