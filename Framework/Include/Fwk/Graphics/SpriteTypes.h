#pragma once

#include "Lib/Math/Vector.h"

enum class Pivot {
    Center,
    TopLeft,
    Top,
    TopRight,
    Left,
    Right,
    BottomLeft,
    Bottom,
    BottomRight,
};

struct SpriteCell
{
    float texcoord[4];
    float pivot[2];
};

struct UVRect
{
    union {
        float m[4];
        struct {
            float left;
            float top;
            float width;
            float height;
        };
    };
};
