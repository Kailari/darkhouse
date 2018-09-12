#include <X11/keysym.h>

#include "config.hpp"

using namespace darkhouse;

Config::Config()
    : window_offset_x(0),
      window_offset_y(0),
      window_w(600),
      window_h(150),
      key_exit(XK_Escape) {}
