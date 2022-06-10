#include "client_options.h"
#include <iostream>
#include "client.h"

// Wczytujemy opcje i włączamy z nimi klienta.
// Jeśli coś pójdzie nie tak, wypisujemy błąd
// na wyjście diagnostyczne i kończymy program
// z kodem błędu 1.

int main(int argc, char **argv) {
    try {
        ClientOptions options(argc, argv);
#ifndef NDEBUG
        std::cerr << options << std::endl;
#endif
        Client client(options);
    } catch (Help &h) {
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what();
    } catch (...) {}
    return 1;
}
