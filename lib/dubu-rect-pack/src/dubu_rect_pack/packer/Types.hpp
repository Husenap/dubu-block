#pragma once

#include <cstdint>

namespace dubu::rect_pack {

struct Size {
    std::uint32_t width  = {};
    std::uint32_t height = {};
};

struct Rect {
    std::uint32_t x = {};
    std::uint32_t y = {};
    std::uint32_t w = {};
    std::uint32_t h = {};

    bool operator==(const Rect& rhs) const {
        return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
    }
};

}  // namespace dubu::rect_pack