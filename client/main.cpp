#include "client_options.h"
#include <iostream>
#include "client.h"

int main(int argc, char **argv) {
    try {
        ClientOptions options(argc, argv);
        std::cerr << options << std::endl;
        Client client(options);
    } catch (std::exception &e) {
        std::cerr << e.what();  
    } catch (...) {}
    return 1;
}
