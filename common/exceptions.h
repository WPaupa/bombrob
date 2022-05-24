#ifndef BOMBOWE_ROBOTY_EXCEPTIONS_H
#define BOMBOWE_ROBOTY_EXCEPTIONS_H

#include <exception>
#include <string>

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
