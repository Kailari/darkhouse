#include <stdlib.h>
#include <string.h>
#include <xcb/shm.h>
#include <xcb/xcb.h>

#include "util.hpp"
#include "window.hpp"

using namespace darkhouse;

Window::Window(xcb_connection_t *c, xcb_screen_t *screen, Config &config)
    : w(config.window_w),
      h(config.window_h),
      x(screen->width_in_pixels / 2 - w / 2 + config.window_offset_x),
      y(screen->height_in_pixels / 2 - h / 2 + config.window_offset_y) {

    // Create foreground context
    m_foreground = xcb_generate_id(c);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t context_values[2];
    context_values[0] = screen->black_pixel;
    context_values[1] = 0;
    xcb_create_gc(c, m_foreground, screen->root, mask, context_values);

    // Generate window ID
    m_window = xcb_generate_id(c);

    // Create the window
    mask = XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
    uint32_t win_values[3];
    win_values[0] = screen->white_pixel;
    win_values[1] = 1;
    win_values[2] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS |
                    XCB_EVENT_MASK_KEY_RELEASE;

    xcb_void_cookie_t window_cookie =
        xcb_create_window(c,                             // connection
                          XCB_COPY_FROM_PARENT,          // depth
                          m_window,                      // window ID
                          screen->root,                  // parent window
                          x, y,                          // x, y
                          w, h,                          // w, h
                          0,                             // border width
                          XCB_WINDOW_CLASS_INPUT_OUTPUT, // class
                          screen->root_visual,           // visual
                          mask, win_values);             // masks

    if (check_cookie(window_cookie, c, "Window creation failed.")) {
        return;
    }

    // Set window type
    xcb_intern_atom_cookie_t atom_cookie =
        xcb_intern_atom(c, 0, WINDOW_TYPE_PROP_LEN, WINDOW_TYPE_PROP);
    xcb_intern_atom_reply_t *atom_reply =
        xcb_intern_atom_reply(c, atom_cookie, nullptr);

    if (!atom_reply) {
        fprintf(stderr, "Unable to set window type.\n");
    } else {
        xcb_atom_t window_type_dock_atom, window_type_atom = atom_reply->atom;
        free(atom_reply);

        atom_cookie =
            xcb_intern_atom(c, 0, WINDOW_TYPE_PROP_LEN, WINDOW_TYPE_PROP);
        atom_reply = xcb_intern_atom_reply(c, atom_cookie, nullptr);
        if (!atom_reply) {
            fprintf(stderr, "Unable to set window type.\n");
        } else {
            free(atom_reply);

            xcb_change_property_checked(c, XCB_PROP_MODE_REPLACE, m_window,
                                        window_type_atom, XCB_ATOM_ATOM, 32, 1,
                                        &window_type_dock_atom);
        }
    }

    // Demand focus
    xcb_atom_t state_atom, attention_state_atom;
    atom_cookie = xcb_intern_atom(c, 0, WM_STATE_PROP_LEN, WM_STATE_PROP);
    atom_reply = xcb_intern_atom_reply(c, atom_cookie, NULL);
    if (!atom_reply) {
        fprintf(stderr, "Unable to demand focus.\n");
    } else {
        state_atom = atom_reply->atom;
        free(atom_reply);
        atom_cookie = xcb_intern_atom(c, 0, WM_STATE_ATTENTION_PROP_LEN,
                                      WM_STATE_ATTENTION_PROP);
        atom_reply = xcb_intern_atom_reply(c, atom_cookie, NULL);
        if (!atom_reply) {
            fprintf(stderr, "Unable to demand focus.\n");
        } else {
            attention_state_atom = atom_reply->atom;
            free(atom_reply);
            xcb_change_property_checked(c, XCB_PROP_MODE_REPLACE, m_window,
                                        state_atom, XCB_ATOM_ATOM, 32, 1,
                                        &attention_state_atom);
        }
    }

    xcb_map_window(c, m_window);
    xcb_flush(c);
}

Window::~Window() {}
