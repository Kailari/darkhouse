#pragma once

#include <xcb/xcb.h>

namespace darkhouse {
namespace Input {
/**
 * Initializes keysyms so that they can later be populated and queried
 */
void init_keysyms(xcb_connection_t *c);

xcb_keysym_t lookup_keysym(xcb_keycode_t keycode, uint16_t modifiers);
} // namespace Input
} // namespace darkhouse