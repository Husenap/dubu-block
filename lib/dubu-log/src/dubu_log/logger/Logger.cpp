#include "Logger.h"

namespace dubu::log::internal {

std::unique_ptr<ILogger> Logger::ourLogger = std::make_unique<NullLogger>();

}