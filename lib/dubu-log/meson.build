dubu_log = static_library('dubu-log',
  [
    'src/dubu_log/logger/ConsoleLogger.cpp',
    'src/dubu_log/logger/FileLogger.cpp',
    'src/dubu_log/logger/Logger.cpp',
    'src/dubu_log/logger/MemoryLogger.cpp',
  ],
  cpp_pch: 'pch/pch.h')

dubu_log_dep = declare_dependency(
  link_with: dubu_log,
  include_directories: include_directories('./src')
)