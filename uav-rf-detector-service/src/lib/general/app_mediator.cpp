#include "app_mediator.h"
#include "lib/utils/logger.h"
#include "lib/server/async_grpc_server.h"
#include "lib/detector/detector_factory.h"
#include "lib/detector/i_detector.h"

#include <memory>

void AppMediator::startApplication()
{
	m_detectors.push_back(
		std::unique_ptr<detector::IDetector>(detector::DetectorFactory::makeDetectorEmulator())
	);
	auto server = server::AsyncGrpcServer(*this);

	for (auto& detector : m_detectors) {
		detector->start();
	}
	bool ok =  server.startListening();
}

const std::vector<std::shared_ptr<detector::IDetector>>& AppMediator::getDetectors()
{
	return m_detectors;
}
