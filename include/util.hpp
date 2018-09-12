#pragma once

#include <stdio.h>
#include <xcb/xcb.h>

static inline bool check_cookie(xcb_void_cookie_t cookie, xcb_connection_t *c, const char* msg) {
    xcb_generic_error_t *err = xcb_request_check(c, cookie);
    if (err) {
        fprintf(stderr, "[err:%u] %s\n", err->error_code, msg);
        return true;
    }

    return false;
}
