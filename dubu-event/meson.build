dubu_event = static_library('dubu-event',
  [
    'src/dubu_event/event/EventEmitter.cpp',
  ],
  dependencies: [dubu_util_dep])

dubu_event_dep = declare_dependency(
  link_with: dubu_event,
  dependencies: [dubu_util_dep],
  include_directories: include_directories('./src')
)