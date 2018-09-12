#pragma once

#include "config.hpp"

namespace darkhouse {
class Window;
class Darkhouse {
  private:
    class xcb_connection_t *m_connection;
    class xcb_screen_t *m_screen;
    Window *m_window;

    Config m_config;

    bool m_should_exit;

  public:
    Darkhouse(Config config);
    ~Darkhouse();

    void run();

  private:
    void process_input(xcb_keysym_t keysym);
};
} // namespace darkhouse
