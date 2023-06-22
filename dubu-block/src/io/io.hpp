#pragma once

#include <string_view>
#include <vector>

namespace dubu::block {
std::vector<unsigned char> ReadFile(std::string_view filepath);
}  // namespace dubu::block