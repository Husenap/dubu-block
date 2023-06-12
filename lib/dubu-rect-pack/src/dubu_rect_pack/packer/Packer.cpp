#include "Packer.hpp"

namespace dubu::rect_pack {

Packer::Packer(int width, int height) {
    mSpaces.emplace_back(0, 0, width, height);
}

std::optional<Rect> Packer::Pack(Size rectangle) {
    for (auto it = mSpaces.crbegin(); it != mSpaces.crend(); ++it) {
        auto& space = *it;

        if (!space.CanFitRect(rectangle)) {
            continue;
        }

        const auto [occupied, newSpace1, newSpace2] = space.Split(rectangle);

        mSpaces.erase(it.base() - 1);

        if (newSpace1) {
            mSpaces.emplace_back(std::move(*newSpace1));
            if (newSpace2) {
                mSpaces.emplace_back(std::move(*newSpace2));
            }
        }

        std::sort(std::begin(mSpaces),
                  std::end(mSpaces),
                  [](const Space& lhs, const Space& rhs) {
                      return lhs.Area() > rhs.Area();
                  });

        return occupied;
    }

    return std::nullopt;
}

}  // namespace dubu::rect_pack