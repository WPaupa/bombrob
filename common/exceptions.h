#ifndef BOMBOWE_ROBOTY_EXCEPTIONS_H
#define BOMBOWE_ROBOTY_EXCEPTIONS_H

// Funkcja main korzysta z std::cerr (bo opcje
// mają tylko operator przesłania do strumienia), podczas
// gdy cała reszta kodu korzysta z makra DEBUG
// (wykorzystującego fprintf), bo ładniej wygląda
// wypisywanie całego ciągu jedną komendą
// (w obydwu przypadkach). Czasami użycie fprintf
// jest wręcz niezbędne, bo chcemy wypisywać
// prefiks ciągu znaków o konkretnej długości.
#include <exception>
#include <string>
#ifndef NDEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) do{} while(0)
#endif

// Wyjątek odpowiadający otrzymaniu złej wiadomości.
class WrongMessage : std::exception {
private:
    std::string msg;
public:
    explicit WrongMessage(const char *msg) : msg(msg) {}

    [[nodiscard]] const char *what() const noexcept override {
        return msg.c_str();
    }
};

class Help : std::exception {
public:
    Help() = default;
    [[nodiscard]] const char *what() const noexcept override {
        return "";
    }
};

#endif //BOMBOWE_ROBOTY_EXCEPTIONS_H
