#pragma once

#include <stdint.h>
#include <xcb/xcb.h>

namespace darkhouse {
struct Config {
    uint16_t window_offset_x;
    uint16_t window_offset_y;
    uint16_t window_w;
    uint16_t window_h;

    xcb_keysym_t key_exit;

    Config();
};
} // namespace darkhouse
