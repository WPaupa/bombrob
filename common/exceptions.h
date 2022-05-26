#ifndef BOMBOWE_ROBOTY_EXCEPTIONS_H
#define BOMBOWE_ROBOTY_EXCEPTIONS_H

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

#endif //BOMBOWE_ROBOTY_EXCEPTIONS_H
