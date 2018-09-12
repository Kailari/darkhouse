#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>

#include "darkhouse.hpp"
#include "util.hpp"
#include "window.hpp"
#include "input.hpp"

using namespace darkhouse;

Darkhouse::Darkhouse(Config config) {
    // Establish connection
    int preferred_screen = 0;
    m_connection = xcb_connect(nullptr, &preferred_screen);

    // Initialize keyboard input
    Input::init_keysyms(m_connection);

    // Try to find screen
    xcb_screen_iterator_t iter =
        xcb_setup_roots_iterator(xcb_get_setup(m_connection));
    xcb_screen_t *fallback_screen = iter.data;
    for (; iter.rem; --preferred_screen, xcb_screen_next(&iter)) {
        if (preferred_screen == 0) {
            m_screen = iter.data;
            break;
        }
    }

    // Make sure we have a valid screen
    if (!m_screen) {
        fprintf(stdout, "Unable to detect preferred screen. Falling back to "
                        "first available.");
        m_screen = fallback_screen;
        if (!m_screen) {
            fprintf(stderr, "Unable to find a valid screen. Aborting.");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize window
    m_window = new Window(m_connection, m_screen, config);

    xcb_flush(m_connection);
}

Darkhouse::~Darkhouse() {
    xcb_disconnect(m_connection);
    delete m_window;
    m_connection = nullptr;
    m_screen = nullptr;
    m_window = nullptr;
}

void Darkhouse::run() {
    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(m_connection))) {

        switch (e->response_type & ~0x00) {
        case XCB_EXPOSE: {
            auto focus_cookie =
                xcb_set_input_focus(m_connection, XCB_INPUT_FOCUS_POINTER_ROOT,
                                    m_window->get_window(), XCB_CURRENT_TIME);
            check_cookie(focus_cookie, m_connection,
                         "Unable to force input focus.");

            xcb_flush(m_connection);
            break;
        }
        case XCB_KEY_PRESS: {
            auto evt = (xcb_key_press_event_t *)e;
            printf("Key pressed: %d\n", evt->detail);
            Input::lookup_keysym(evt->detail, evt->state);
            break;
        }
        case XCB_KEY_RELEASE: {
            auto evt = (xcb_key_release_event_t *)e;
            printf("Key released: %d\n", evt->detail);
            break;
        }
        default:
            break;
        }
        free(e);
    }
}
