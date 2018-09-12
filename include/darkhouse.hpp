#pragma once

#include "config.hpp"

namespace darkhouse {
class Window;
class Darkhouse {
  private:
    class xcb_connection_t *m_connection;
    class xcb_screen_t *m_screen;
    Window *m_window;

  public:
    Darkhouse(Config config);
    ~Darkhouse();

    void run();
};
} // namespace darkhouse
