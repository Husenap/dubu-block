#pragma once

#include <chrono>
#include <string_view>

#include <dubu_log/dubu_log.h>

namespace dubu::block {

class Timer {
public:
  Timer(std::string_view label)
      : label(label) {
    t0 = std::chrono::high_resolution_clock::now();
  }
  ~Timer() {
    const auto t1 = std::chrono::high_resolution_clock::now();
    DUBU_LOG_DEBUG("Timer[{}]: {}ms",
                   label,
                   std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.f);
  }

private:
  std::string_view                               label;
  std::chrono::high_resolution_clock::time_point t0;
};

}  // namespace dubu::block