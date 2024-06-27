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

public:
	/*!
	 * @brief Оставлено как затычка - нужно будет переделать, если
	 * лайфтайм детектора не будет совпадать с лайфтаймом сервсиа.
	 */
	std::thread computeThread;

};

}

#endif //DETECTOR_EMULATOR_H
