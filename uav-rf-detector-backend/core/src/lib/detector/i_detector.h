#ifndef I_DETECTOR_H
#define I_DETECTOR_H

#include <boost/signals2.hpp>
#include "rfdetector.grpc.pb.h"

namespace detector {

/*!
 * @brief Базовый класс для всех детекторов.
 * Детектор не должен блокировать выполнение главного потока
 * (сложные вычисления должны быть вынесены в отдельный поток).
 */
class IDetector {

public:
	virtual ~IDetector() = default;
	virtual bool start() = 0;

public:
	boost::signals2::signal<void (rfdetector::Detection detection)> detectionFound;
};

}

#endif //I_DETECTOR_H
