#ifndef DETECTOR_EMULATOR_H
#define DETECTOR_EMULATOR_H

#include "lib/detector/i_detector.h"

namespace detector
{

/*!
 * @brief Представляет из себя детектор, предоставляющий свои данные,
 * читая и анализируя БД сигналов из файла.
 */
class DetectorEmulator : public IDetector
{
public:
	bool start() override;

};

}

#endif //DETECTOR_EMULATOR_H
