#include "FileLogger.h"

namespace dubu::log {

FileLogger::FileLogger(const std::string& file) {
  mStream.open(file);
}

void FileLogger::InternalLog(LogLevel           level,
                             const std::string& file,
                             uint32_t           line,
                             const std::string& function,
                             const std::string& text) {
  mStream << std::format("[{}]: {}:{}:{}: {}", LogLevelString(level), file, line, function, text)
          << std::endl;
}

}  // namespace dubu::log
