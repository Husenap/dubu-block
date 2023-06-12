#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

#include "format.hpp"

namespace dubu::log {

enum class LogLevel { Debug, Info, Warning, Error, Fatal, Count };
inline const char* LogLevelString(LogLevel level) {
  constexpr static std::array<const char*, static_cast<std::size_t>(LogLevel::Count)> Strings{
      "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

  return Strings[static_cast<std::size_t>(level)];
}

class LogError : public std::exception {
public:
  LogError(std::string_view message)
      : mMessage(message) {}

  char const* what() const noexcept override { return mMessage.c_str(); }

private:
  std::string mMessage;
};

class ILogger {
public:
  virtual ~ILogger() = default;

  void SetLevel(LogLevel level) { mLevel = level; }

  template <typename... Args>
  void Log(dubu::log::LogLevel level,
           const char*         file,
           uint32_t            line,
           const char*         function,
           const char*         formatString,
           Args&&... args) {
    if (level < mLevel) {
      return;
    }

    const std::string fileName = std::filesystem::path(file).filename().string();

    std::string functionName = function;
    if (auto pos = functionName.find("lambda"); pos != std::string::npos) {
      functionName = functionName.substr(0, pos + 6) + ">";
    }
    if (auto pos = functionName.find_last_of(":"); pos != std::string::npos) {
      functionName = functionName.substr(pos + 1);
    }

    const auto text = shared::format(formatString, std::forward<Args>(args)...);

    InternalLog(level, fileName, line, functionName, text);

    if (level == LogLevel::Fatal) {
      throw LogError(text);
    }
  }

protected:
  virtual void InternalLog(LogLevel           level,
                           const std::string& file,
                           uint32_t           line,
                           const std::string& function,
                           const std::string& text) = 0;

  LogLevel mLevel;
};

}  // namespace dubu::log