#include "server_options.h"
#include <iostream>

int main(int argc, char **argv) {
    try {
        ServerOptions options(argc, argv);
        std::cerr << options;
    } catch (...) {}
}