#pragma once

#include "ILogger.h"

namespace dubu::log::internal {

class NullLogger : public ILogger {
protected:
	void InternalLog(LogLevel,
	                 const std::string&,
	                 uint32_t,
	                 const std::string&,
	                 const std::string&) override {}
};

class Logger {
public:
	static void RegisterLogger(std::unique_ptr<ILogger>&& logger) {
		ourLogger = std::move(logger);
	}

	static ILogger& Get() { return *ourLogger; }

private:
	Logger()  = delete;
	~Logger() = delete;

	static std::unique_ptr<ILogger> ourLogger;
};

}  // namespace dubu::log::internal

namespace dubu::log {
template <typename T, typename... Args>
std::enable_if_t<std::is_base_of_v<ILogger, T>> Register(Args&&... args) {
	dubu::log::internal::Logger::RegisterLogger(
	    std::make_unique<T>(std::forward<Args>(args)...));
}
}  // namespace dubu::log

#ifndef _DUBU_LOG_GENERAL
#	ifndef DUBU_LOG_DISABLED

// clang-format off
#		define _DUBU_LOG_GENERAL(_level, ...) dubu::log::internal::Logger::Get().Log(_level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

#		define DUBU_LOG_DEBUG(...) _DUBU_LOG_GENERAL(dubu::log::LogLevel::Debug, __VA_ARGS__)
#		define DUBU_LOG_INFO(...) _DUBU_LOG_GENERAL(dubu::log::LogLevel::Info, __VA_ARGS__)
#		define DUBU_LOG_WARNING(...) _DUBU_LOG_GENERAL(dubu::log::LogLevel::Warning, __VA_ARGS__)
#		define DUBU_LOG_ERROR(...) _DUBU_LOG_GENERAL(dubu::log::LogLevel::Error, __VA_ARGS__)
#		define DUBU_LOG_FATAL(...) _DUBU_LOG_GENERAL(dubu::log::LogLevel::Fatal, __VA_ARGS__)
// clang-format on

#	else

#		define _DUBU_LOG_GENERAL(...)
#		define DUBU_LOG_DEBUG(...)
#		define DUBU_LOG_INFO(...)
#		define DUBU_LOG_WARNING(...)
#		define DUBU_LOG_ERROR(...)
#		define DUBU_LOG_FATAL(...)

#	endif
#endif
