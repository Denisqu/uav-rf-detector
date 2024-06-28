#ifndef DETECTOR_FACTORY_H
#define DETECTOR_FACTORY_H

#include <memory>

namespace detector { class IDetector; }

namespace detector
{

class DetectorFactory
{
public:
	static IDetector* makeDetectorEmulator();
};

}

#endif //DETECTOR_FACTORY_H