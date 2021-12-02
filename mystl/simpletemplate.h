#pragma once

template <typename T>
class SimpleT {
public:
    T add(T a, T b) {
        return a+b;
    }

    SimpleT<T> multiply(T a, T b);
};
