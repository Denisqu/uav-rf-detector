#include "detector_emulator.h"
#include "lib/utils/logger.h"

#include <chrono>
#include <thread>

namespace {

LOG_CAT(log_cat, "detector.detector_emulator")

}

bool detector::DetectorEmulator::start()
{
	computeThread = std::thread {[this](){
		while (true) {
			uint32_t i = 0;
			std::this_thread::sleep_for(std::chrono::seconds{ 1 });
			rfdetector::Detection detection;
			detection.set_timestamp(123);
			detection.set_carrier(i);
			detection.set_protocol("test");

			log_info(log_cat, "Detection found: {}", detection.DebugString());
			detectionFound(detection);
			++i;
		}
	}};
	computeThread.detach();

	return true;
}
