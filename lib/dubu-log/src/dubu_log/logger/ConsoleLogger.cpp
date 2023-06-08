#include "ConsoleLogger.h"

namespace dubu::log {

void ConsoleLogger::InternalLog(LogLevel           level,
                                const std::string& file,
                                uint32_t           line,
                                const std::string& function,
                                const std::string& text) {
  std::cout << std::format("[{}]: {}:{}:{}: {}", LogLevelString(level), file, line, function, text)
            << std::endl;
}

}  // namespace dubu::log