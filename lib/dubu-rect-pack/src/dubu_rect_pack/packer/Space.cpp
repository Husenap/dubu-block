#include "Space.hpp"

namespace dubu::rect_pack {

Space::Space(std::uint32_t left,
             std::uint32_t top,
             std::uint32_t width,
             std::uint32_t height)
    : mLeft(left)
    , mTop(top)
    , mWidth(width)
    , mHeight(height)
    , mArea(width * height) {}

bool Space::CanFitRect(Size rectangle) const {
    return (rectangle.width <= mWidth) && (rectangle.height <= mHeight);
}

std::tuple<Space, std::optional<Space>, std::optional<Space>> Space::Split(
    Size rectangle) const {
    assert(rectangle.width <= mWidth);
    assert(rectangle.height <= mHeight);

    if (rectangle.width == mWidth && rectangle.height == mHeight) {
        return {*this, std::nullopt, std::nullopt};
    } else if (rectangle.width == mWidth) {
        return {Space(mLeft, mTop, mWidth, rectangle.height),
                Space(mLeft,
                      mTop + rectangle.height,
                      mWidth,
                      mHeight - rectangle.height),
                std::nullopt};
    } else if (rectangle.height == mHeight) {
        return {Space(mLeft, mTop, rectangle.width, mHeight),
                Space(mLeft + rectangle.width,
                      mTop,
                      mWidth - rectangle.width,
                      mHeight),
                std::nullopt};
    } else {
        Space newSpace(mLeft, mTop, rectangle.width, rectangle.height);
        Space bottomSpace(
            mLeft, mTop + rectangle.height, mWidth, mHeight - rectangle.height);
        Space rightSpace(
            mLeft + rectangle.width, mTop, mWidth - rectangle.width, mHeight);

        if (bottomSpace.Area() >= rightSpace.Area()) {
            return {newSpace,
                    bottomSpace,
                    Space(mLeft + rectangle.width,
                          mTop,
                          mWidth - rectangle.width,
                          rectangle.height)};
        } else {
            return {newSpace,
                    rightSpace,
                    Space(mLeft,
                          mTop + rectangle.height,
                          rectangle.width,
                          mHeight - rectangle.height)};
        }
    }
}

}  // namespace dubu::rect_pack