#pragma once

#include <xcb/xcb.h>

#include "config.hpp"

#define WINDOW_TYPE_PROP "_NET_WM_WINDOW_TYPE"
#define WINDOW_TYPE_PROP_LEN 19

#define WM_STATE_PROP "_NET_WM_STATE"
#define WM_STATE_PROP_LEN 13

#define WM_STATE_ATTENTION_PROP "_NET_WM_STATE_DEMANDS_ATTENTION"
#define WM_STATE_ATTENTION_PROP_LEN 31

namespace darkhouse {
/**
 * Manages the xserver connection. Creates a dock-type window with
 * CW_OVERRIDE_REDIRECT -flag, (hopefully) telling the WM to not to tamper with
 * our window.
 */
class Window {
  private:
    // General window/connection stuff
    xcb_window_t m_window;
    xcb_gcontext_t m_foreground;

  public:
    const uint16_t w;
    const uint16_t h;
    const uint16_t x;
    const uint16_t y;

    inline xcb_window_t get_window() { return m_window; }
    inline xcb_gcontext_t get_foreground() { return m_foreground; }

    Window(xcb_connection_t *c, xcb_screen_t *screen, Config &config);
    ~Window();
};
} // namespace darkhouse