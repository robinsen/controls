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

template<typename T>
bool Compare<T>::isEqual(const T& t1, const T& t2)
{
    std::cout<<" T equal "<<std::endl;
    return t1 == t2;
}

bool Compare<double>::isEqual(const double& d1, const double& d2)
{
    std::cout<<" double equal "<<std::endl;
    return (abs(d1 - d2) < 10e-6);
}

bool Compare<float>::isEqual(const float& f1, const float& f2)
{
    std::cout<<" float equal "<<std::endl;
    return (abs(f1 - f2) < 10e-6);
}