#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

#define XK_MISCELLANY // For function and modifier keys
#include <X11/Xlib.h>
#include <X11/keysymdef.h>

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

bool get_case_sensitive_variants(xcb_keysym_t keysym, xcb_keysym_t *upper,
                                 xcb_keysym_t *lower) {
    // TODO: Implement
    return false;
}

} // namespace Input
} // namespace darkhouse

using namespace darkhouse;

Input::Modifiers::Modifiers(uint8_t modifiers) {
    shift = (modifiers & XCB_MOD_MASK_SHIFT) != 0;
    lock = (modifiers & XCB_MOD_MASK_LOCK) != 0;
    control = (modifiers & XCB_MOD_MASK_CONTROL) != 0;
    mode_switch = mod1 = (modifiers & XCB_MOD_MASK_1) != 0;
    num = mod2 = (modifiers & XCB_MOD_MASK_2) != 0;
    mod3 = (modifiers & XCB_MOD_MASK_3) != 0;
    mod4 = (modifiers & XCB_MOD_MASK_4) != 0;
    mod5 = (modifiers & XCB_MOD_MASK_5) != 0;

    // TODO: Figure out where mode-switch maps to by default, now its guessed to mod1
}

void Input::init_keysyms(xcb_connection_t *c) {
    xcb_keycode_t first = xcb_get_setup(c)->min_keycode;
    uint8_t count = xcb_get_setup(c)->max_keycode - first + 1;

    Input::c = c;
    Input::mapping_cookie = xcb_get_keyboard_mapping(c, first, count);
}

void Input::finalize() {
    if (has_mapping_reply()) {
        free(reply);
        reply = nullptr;
    }
}

xcb_keysym_t Input::handle_input(xcb_keycode_t keycode,
                                 Input::Modifiers modifiers) {
    update_mapping_reply();

    xcb_keysym_t *keysyms = xcb_get_keyboard_mapping_keysyms(reply);
    xcb_keycode_t min_keycode = xcb_get_setup(c)->min_keycode;
    xcb_keycode_t max_keycode = xcb_get_setup(c)->max_keycode;

    uint8_t keysyms_per_keycode = reply->keysyms_per_keycode;
    keysyms = &keysyms[(keycode - min_keycode) * keysyms_per_keycode];

    // Process the list
    for (int i = keysyms_per_keycode; i >= 0; i--) {
        // Trailing NoSymbol entries can be ignored
        if (keysyms[i] == XCB_NO_SYMBOL) {
            // If list was empty, return null symbol
            if (i == 0) {
                return XCB_NO_SYMBOL;
            }
            continue;
        }

        // If the list is a pair of keys, treat list as:
        //  K1 K2 K1 K2
        if (i == 1) {
            keysyms[2] = keysyms[0];
            keysyms[3] = keysyms[1];
            break;
        }
        // If the list is a single key, treat list as:
        //  K NoSymbol K NoSymbol
        else if (i == 0) {
            keysyms[2] = keysyms[0];
            break;
        }
    }

    // If second sym in group is NoSymbol, it should be treated as being the
    // same as the first sym
    if (keysyms[0] && !keysyms[1]) {
        // ...except if first sym is alphabetic with both lower- and uppercase
        // forms defined, where first should be treated as lowercase and second
        // uppercase
        xcb_keysym_t upper, lower;
        if (get_case_sensitive_variants(keysyms[0], &upper, &lower)) {
            keysyms[0] = lower;
            keysyms[1] = upper;
        } else {
            keysyms[1] = keysyms[0];
        }
    }

    // Same for group 2
    if (keysyms[2] && !keysyms[3]) {
        xcb_keysym_t upper, lower;
        if (get_case_sensitive_variants(keysyms[0], &upper, &lower)) {
            keysyms[2] = lower;
            keysyms[3] = upper;
        } else {
            keysyms[3] = keysyms[2];
        }
    }

    // These might not be accurate, figured out with one keyboard layout just
    // by trial and error. Might break on other keyboard configs.
    // TODO: Allow configuring which mods map to which modifier

    // mode switch OFF + no mod     => 1. sym => keysyms[0]
    // mode switch OFF + caps/shift => 2. sym => keysyms[1]
    // mode switch ON  + no mod     => 3. sym => keysyms[2]
    // mode switch ON  + caps/shift => 4. sym => keysyms[3]
    // mod5                         => 5. sym => keysyms[4]
    // mod5 + shift                 => 6. sym => keysyms[5]
    // mod5                         => 7. sym => keysyms[6]

    int index = 0;
    if (modifiers.mode_switch) index += 2;
    if (modifiers.mod5) index += 4;
    if (modifiers.shift || modifiers.lock) index += 1;

    // Just-in-case don't limit to hardcoded 6, but use per instead
    if (index > keysyms_per_keycode) index = keysyms_per_keycode;

    for (int i = 0; i < 7; i++) {
        printf("  %d: %d/%s", i, keysyms[i], XKeysymToString(keysyms[i]));
    }
    putchar('\n');
    printf("  i: %d, raw: %d\n", index, keysyms[index]);
    printf("  resolved string: %s\n", XKeysymToString(keysyms[index]));

    return keysyms[index];
}
