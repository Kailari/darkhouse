#pragma once

#include <cairo/cairo.h>
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
 * Manages the xserver connection and contains cairo surface/context. Creates
 * a dock-type window with CW_OVERRIDE_REDIRECT -flag set, (hopefully) telling
 * the WM to ignore our window. This should result in an alwaus-on-top window
 * without decorations
 */
class Window {
  private:
    // General window/connection stuff
    xcb_window_t m_window;

    // Cairo
    cairo_surface_t *m_cairo_surface;
    cairo_t *m_cairo_context;

  public:
    const uint16_t w;
    const uint16_t h;
    const uint16_t x;
    const uint16_t y;

    xcb_window_t get_window();
    cairo_surface_t *get_surface();
    cairo_t *get_context();

    Window(xcb_connection_t *c, xcb_screen_t *screen, Config &config);
    ~Window();

    void demand_focus(xcb_connection_t *c);
};
} // namespace darkhouse