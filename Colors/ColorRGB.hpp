#pragma once

namespace moe {

struct ColorRGB {
    ColorRGB(float r, float g, float b)
            : r{r}, g{g}, b{b} {}

    ColorRGB()
            : r{0}, g{0}, b{0} {}

    float r;
    float g;
    float b;

    static ColorRGB black() { return ColorRGB{ 0, 0, 0 }; }
    static ColorRGB white() { return ColorRGB{ 1, 1, 1 };}
};
}
