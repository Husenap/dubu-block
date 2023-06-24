#pragma once

namespace dubu::block {

template <class T>
class Singleton {
public:
  inline static T& Get() {
    static T our_instance;
    return our_instance;
  }
};

}  // namespace dubu::block