#include <stdio.h>

#include "config.hpp"
#include "darkhouse.hpp"

using namespace darkhouse;

int main(int argc, char const *argv[]) {
    Config config;

    Darkhouse dh(config);
    dh.run();

    return 0;
}
