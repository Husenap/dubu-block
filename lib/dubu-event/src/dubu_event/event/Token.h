#pragma once

#include <memory>

namespace dubu::event {

namespace internal {
struct _Token {};
}  // namespace internal

using Token = std::shared_ptr<internal::_Token>;

}  // namespace dubu::event