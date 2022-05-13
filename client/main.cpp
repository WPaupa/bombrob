#include "client_options.h"
#include <iostream>

int main(int argc, char **argv) {
    try {
        ClientOptions options(argc, argv);
        std::cout << options;
    } catch (...) {throw;}
}