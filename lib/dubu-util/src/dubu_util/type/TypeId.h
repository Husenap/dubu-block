#pragma once

#include <cstdint>

namespace dubu::util {

using IdType = uint16_t;

class TypeId {
public:
  template <typename T>
  static IdType Get() {
    static IdType id = ++GlobalCounter;
    return id;
  }

  template <typename T>
  static IdType Get(const T&) {
    return Get<T>();
  }

private:
  static IdType GlobalCounter;
};

}  // namespace dubu::util