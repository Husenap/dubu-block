#pragma once

#include "Types.hpp"

namespace dubu::rect_pack {

class Space {
public:
    explicit Space(std::uint32_t left,
                   std::uint32_t top,
                   std::uint32_t width,
                   std::uint32_t height);

    bool CanFitRect(Size rectangle) const;

    std::tuple<Space, std::optional<Space>, std::optional<Space>> Split(
        Size rectangle) const;

    std::uint32_t Area() const { return mArea; }

    operator Rect() const { return Rect{mLeft, mTop, mWidth, mHeight}; }

private:
    std::uint32_t mLeft;
    std::uint32_t mTop;
    std::uint32_t mWidth;
    std::uint32_t mHeight;
    std::uint32_t mArea;
};

}  // namespace dubu::rect_pack