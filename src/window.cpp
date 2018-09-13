#include <cairo/cairo-xcb.h>
#include <cairo/cairo.h>
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

    // Generate window ID
    m_window = xcb_generate_id(c);

    // Create the window
    uint32_t mask =
        XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
    uint32_t win_values[3];
    win_values[0] = screen->white_pixel;
    win_values[1] = 1;
    win_values[2] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                    XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

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

    // Find correct visual
    xcb_visualtype_t *visual = nullptr;
    auto depth_iter = xcb_screen_allowed_depths_iterator(screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        auto visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (screen->root_visual == visual_iter.data->visual_id) {
                visual = visual_iter.data;
                break;
            }
        }
    }
    if (!visual) {
        fprintf(stderr, "Could not determine visual");
        exit(EXIT_FAILURE);
    }

    // Create cairo surface
    m_cairo_surface = cairo_xcb_surface_create(c, m_window, visual, w, h);
    if (!m_cairo_surface) {
        fprintf(stderr, "Could not create cairo surface");
        exit(EXIT_FAILURE);
    }

    m_cairo_context = cairo_create(m_cairo_surface);
    if (!m_cairo_context) {
        fprintf(stderr, "Could not create cairo context");
        exit(EXIT_FAILURE);
    }

    xcb_map_window(c, m_window);

    // Demand focus
    demand_focus(c);

    xcb_flush(c);
}

Window::~Window() {
    if (m_cairo_surface) {
        cairo_surface_destroy(m_cairo_surface);
        m_cairo_surface = nullptr;
    }

    if (m_cairo_context) {
        cairo_destroy(m_cairo_context);
        m_cairo_context = nullptr;
    }
}

xcb_window_t Window::get_window() { return m_window; }

cairo_surface_t *Window::get_surface() { return m_cairo_surface; }

cairo_t *Window::get_context() { return m_cairo_context; }

void Window::demand_focus(xcb_connection_t *c) {
    // Demand attention
    xcb_atom_t state_atom, attention_state_atom;
    xcb_intern_atom_cookie_t atom_cookie =
        xcb_intern_atom(c, 0, WM_STATE_PROP_LEN, WM_STATE_PROP);
    xcb_intern_atom_reply_t *atom_reply =
        xcb_intern_atom_reply(c, atom_cookie, NULL);
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

    // Demand input focus
    auto focus_cookie = xcb_set_input_focus(c, XCB_INPUT_FOCUS_POINTER_ROOT,
                                            m_window, XCB_CURRENT_TIME);
    check_cookie(focus_cookie, c, "Unable to force input focus.");
}
