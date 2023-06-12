#pragma once

#include "Space.hpp"

namespace dubu::rect_pack {

class Packer {
public:
    Packer(int width, int height);

    std::optional<Rect> Pack(Size rectangle);

private:
    std::vector<Space> mSpaces;
};

}  // namespace dubu::rect_pack