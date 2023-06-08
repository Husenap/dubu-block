#include "ConsoleLogger.h"

#include <iostream>

#include <fmt/ostream.h>

namespace dubu::log {

void ConsoleLogger::InternalLog(LogLevel           level,
                                const std::string& file,
                                uint32_t           line,
                                const std::string& function,
                                const std::string& text) {
  std::cout << fmt::format("[{}]: {}:{}:{}: {}", LogLevelString(level), file, line, function, text)
            << std::endl;
}

}  // namespace dubu::log