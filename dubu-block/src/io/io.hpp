#pragma once

#include <string>
#include <string_view>

namespace dubu::block {
std::string read_file(std::string_view filepath);
}  // namespace dubu::block