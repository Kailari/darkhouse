#pragma once

#include <stdint.h>

namespace darkhouse {
struct Config {
    uint16_t window_offset_x;
    uint16_t window_offset_y;
    uint16_t window_w;
    uint16_t window_h;

    Config();
};
} // namespace darkhouse
