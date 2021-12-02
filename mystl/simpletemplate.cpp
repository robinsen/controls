#include <iostream>
#include <memory>
#include <cstring>
#include "simpletemplate.h"
using namespace std;

template <typename T>
SimpleT<T> SimpleT<T>::multiply(T a, T b)
{
    return a*b;
}