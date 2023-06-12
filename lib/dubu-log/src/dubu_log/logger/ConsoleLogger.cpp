#include "ConsoleLogger.h"

#include <iostream>

#include "format.hpp"

namespace dubu::log {

void ConsoleLogger::InternalLog(LogLevel           level,
                                const std::string& file,
                                uint32_t           line,
                                const std::string& function,
                                const std::string& text) {
  dubu::log::format_to(
      std::cout, "[{}]: {}:{}:{}: {}\n", LogLevelString(level), file, line, function, text);
}

}  // namespace dubu::log