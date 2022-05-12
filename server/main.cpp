#include "server_options.h"
#include <iostream>

int main(int argc, char **argv) {
    try {
        server_options options(argc, argv);
        std::cout << options;
    } catch (...) {}
}