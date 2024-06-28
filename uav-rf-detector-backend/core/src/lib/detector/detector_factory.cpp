#include "detector_factory.h"
#include "detector_emulator.h"

detector::IDetector *detector::DetectorFactory::makeDetectorEmulator()
{
	auto detector = new DetectorEmulator();
	return detector;
}
