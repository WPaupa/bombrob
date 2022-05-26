#include "client_options.h"
#include <iostream>
#include "client.h"

// Wczytujemy opcje i włączamy z nimi klienta.
// Jeśli coś pójdzie nie tak, wypisujemy błąd
// na wyjście diagnostyczne i kończymy program
// z kodem błędu 1.

// Funkcja main korzysta z std::cerr, podczas
// gdy cała reszta kodu korzysta z makra DEBUG
// (wykorzystującego fprintf), bo ładniej wygląda
// wypisywanie całego ciągu jedną komendą
// (w obydwu przypadkach).
int main(int argc, char **argv) {
    try {
        ClientOptions options(argc, argv);
#ifndef NDEBUG
        std::cerr << options << std::endl;
#endif
        Client client(options);
    } catch (std::exception &e) {
        std::cerr << e.what();
    } catch (...) {}
    return 1;
}
