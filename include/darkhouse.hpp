#pragma once

#include <xcb/xcb.h>

#include "config.hpp"

namespace darkhouse {
class Darkhouse {
  private:
    xcb_connection_t *m_connection;
    xcb_screen_t *m_screen;

    class Window* m_window;

  public:
    Darkhouse(Config config);
    ~Darkhouse();

    void run();
};
} // namespace darkhouse
