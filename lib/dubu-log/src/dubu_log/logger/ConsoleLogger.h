#pragma once

#include "ILogger.h"

namespace dubu::log {
class ConsoleLogger : public ILogger {
protected:
  void InternalLog(LogLevel           level,
                   const std::string& file,
                   uint32_t           line,
                   const std::string& function,
                   const std::string& text) override;
};
}  // namespace dubu::log