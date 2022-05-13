#ifndef BOMBOWE_ROBOTY_VARIANT_H
#define BOMBOWE_ROBOTY_VARIANT_H
#include <memory>
#include <functional>

template <class Base, typename ... Ts>
class Variant {
public:
    static std::unique_ptr<Base> createObject(int i) {
        if (i < sizeof...(Ts)) {
            static const std::function<std::unique_ptr<Base>()> fs[] = {
                    [](){ return std::make_unique<Ts>();}...
            };
            return fs[i]();
        }
        throw std::runtime_error("Invalid arg");
    }

};


#endif //BOMBOWE_ROBOTY_VARIANT_H
