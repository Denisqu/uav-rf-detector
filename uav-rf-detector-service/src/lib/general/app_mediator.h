#ifndef APP_MEDIATOR_H
#define APP_MEDIATOR_H

namespace detector { class IDetector; }

#include <memory>
#include <vector>

/*!
 * @brief todo...
 */
class AppMediator
{
public:
	void startApplication();
	const std::vector<std::shared_ptr<detector::IDetector>>& getDetectors();

private:
	std::vector<std::shared_ptr<detector::IDetector>> m_detectors;
};

#endif //APP_MEDIATOR_H
