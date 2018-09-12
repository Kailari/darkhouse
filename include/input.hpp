#pragma once

#include <xcb/xcb.h>

namespace darkhouse {
namespace Input {
struct Modifiers {
    bool caps;
    bool num;
    bool mode_switch;

    bool shift;
    bool lock;
    bool control;
    bool mod1;
    bool mod2;
    bool mod3;
    bool mod4;
    bool mod5;

    Modifiers(uint8_t modifiers);
};

/**
 * Initializes keysyms so that they can later be populated and queried
 */
void init_keysyms(xcb_connection_t *c);

/**
 * Handles incoming keyboard input. Resolves what keysym is in question based on
 * keycode and modifiers.
 */
xcb_keysym_t handle_input(xcb_keycode_t keycode, Modifiers modifiers);

void finalize();
} // namespace Input
} // namespace darkhouse