#include "server_options.h"
#include "server.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    try {
        ServerOptions options(argc, argv);
#ifndef NDEBUG
        std::cerr << options << std::endl;
#endif
        Server server(options);
    } catch (Help &h) {
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what();
    } catch (...) {
        std::cerr << "Unknown exception caught";
    }
    return 1;
}
