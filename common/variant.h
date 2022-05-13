#ifndef BOMBOWE_ROBOTY_VARIANT_H
#define BOMBOWE_ROBOTY_VARIANT_H
#include <memory>
#include <functional>
#include <variant>
#include <concepts>
#include <boost/asio.hpp>
#include "types.h"

template <class Base, net_type ... Ts>
requires (std::is_base_of_v<Base, Ts> && ...)
class Variant {
private:
    std::variant<Ts ...> data;
public:
    void create(int i) {
        if (i < sizeof...(Ts)) {
            static const std::function<std::unique_ptr<Base>()> fs[] = {
                    [](){ return std::make_unique<Ts>();} ...
            };
            data = fs[i]().get();
        }
        throw std::runtime_error("Invalid arg");
    }

    template<typename T>
    requires (std::is_same_v<T, Ts> || ...)
    T get() {
        return data.template get<T>();
    }

    void send(boost::asio::ip::tcp::socket &&socket) {
        std::visit([socket = std::move(socket)](auto &&v){
            v.send(std::move(socket));
        }, data);
    }

    void recv(boost::asio::ip::tcp::socket &&socket) {
        std::visit([socket = std::move(socket)](auto &&v){
            v.recv(std::move(socket));
        }, data);
    }

};


#endif //BOMBOWE_ROBOTY_VARIANT_H
