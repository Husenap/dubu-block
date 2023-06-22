#include "FileLogger.h"

#include "format.hpp"

namespace dubu::log {

FileLogger::FileLogger(const std::string& file) {
  mStream.open(file);
}

void FileLogger::InternalLog(LogLevel           level,
                             const std::string& file,
                             uint32_t           line,
                             const std::string& function,
                             const std::string& text) {
  dubu::log::format_to(
      mStream, "[{}]: {}:{}:{}: {}\n", LogLevelString(level), file, line, function, text);
}

}  // namespace dubu::log
