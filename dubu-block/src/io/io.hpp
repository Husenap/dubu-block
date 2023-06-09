#pragma once

#include <string_view>
#include <vector>

namespace dubu::block {
std::vector<unsigned char> read_file(std::string_view filepath);
}  // namespace dubu::block