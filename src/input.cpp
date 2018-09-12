#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>

#define XK_MISCELLANY // For function and modifier keys
#include <X11/keysymdef.h>
#include <X11/Xlib.h>

#include "input.hpp"

namespace darkhouse {
namespace Input {

xcb_connection_t *c;
xcb_get_keyboard_mapping_cookie_t mapping_cookie;
xcb_get_keyboard_mapping_reply_t *reply = nullptr;

inline bool has_mapping_reply() { return reply != nullptr; }

void update_mapping_reply() {
    if (has_mapping_reply())
        return;

    reply = xcb_get_keyboard_mapping_reply(c, mapping_cookie, nullptr);
}

bool mod_caps;
bool mod_num;

} // namespace Input
} // namespace darkhouse

using namespace darkhouse;

void Input::init_keysyms(xcb_connection_t *c) {
    xcb_keycode_t first = xcb_get_setup(c)->min_keycode;
    uint8_t count = xcb_get_setup(c)->max_keycode - first + 1;

    Input::c = c;
    Input::mapping_cookie =
        xcb_get_keyboard_mapping(c, first, count);
}

void Input::finalize() {
    if (has_mapping_reply()) {
        free(reply);
        reply = nullptr;
    }
}

xcb_keysym_t Input::lookup_keysym(xcb_keycode_t keycode, uint16_t modifiers) {
    update_mapping_reply();

    xcb_keysym_t *keysyms = xcb_get_keyboard_mapping_keysyms(reply);
    xcb_keycode_t min_keycode = xcb_get_setup(c)->min_keycode;
    xcb_keycode_t max_keycode = xcb_get_setup(c)->max_keycode;

    uint8_t keysyms_per_keycode = reply->keysyms_per_keycode;
    printf("keysyms_per_keycode: %d\n", keysyms_per_keycode);
    keysyms = &keysyms[(keycode - min_keycode) * keysyms_per_keycode];

    if (keysyms[0] == XK_Num_Lock) {
        printf("NUM LOCK!\n");
    } else if (keysyms[0] == XK_Caps_Lock) {
        printf("CAPS LOCK!\n");
    }

    printf("  raw: %d\n", keysyms[0]);
    printf("  resolved string: %s\n", XKeysymToString(keysyms[0]));


    return keysyms[0];
}
