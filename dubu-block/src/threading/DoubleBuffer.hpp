#pragma once

namespace dubu::block {

template <typename T>
class DoubleBuffer {
public:
  const T& GetReadBuffer() const { return isFlipped ? ping : pong; }
  T&       GetWriteBuffer() { return isFlipped ? ping : pong; }
  void     Flip() { isFlipped = !isFlipped; }

private:
  T    ping, pong;
  bool isFlipped = false;
};

}  // namespace dubu::block