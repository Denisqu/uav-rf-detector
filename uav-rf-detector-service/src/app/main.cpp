#include "lib/utils/logger.h"
#include "lib/general/app_mediator.h"

namespace {

LOG_CAT(log_cat, "main")

}

int main(int argc, const char** argv)
{
	log_info(log_cat, "Start of application");

	auto app = std::make_unique<AppMediator>();
	app->startApplication();


}