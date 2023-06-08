#pragma once

#include <fstream>

#include "ILogger.h"


namespace dubu::log {

class FileLogger : public ILogger {
public:
  FileLogger(const std::string& file);

protected:
  virtual void InternalLog(LogLevel           level,
                           const std::string& file,
                           uint32_t           line,
                           const std::string& function,
                           const std::string& text) override;

private:
  std::ofstream mStream;
};

}  // namespace dubu::log