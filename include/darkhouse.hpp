#pragma once

#include <X11/Xlib.h>

#include "config.hpp"
#include "window.hpp"

namespace darkhouse {
class Darkhouse {
  private:
    Display *m_xlib_display;
    class xcb_connection_t *m_connection;
    class xcb_screen_t *m_screen;

    Window *m_window;

  public:
    Darkhouse(Config config);
    ~Darkhouse();

    void run();
};
} // namespace darkhouse
