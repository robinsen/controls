#pragma once
#include <iostream>
#include <memory>
namespace DrawStuff{

class DrawImpl {
public:
    DrawImpl(){}
    void draw(const int& x, const int& y)
    {
        std::cout<<"impl draw x="<<x<<" y="<<y<<std::endl;
    }
};

class Draw {
public:
    Draw()
    :impl(new DrawImpl)
    {

    }

    void swap(Draw& d)
    {
        using std::swap;
        swap(impl, d.impl);
    }

    void draw(const int& x, const int& y)
    {
        std::cout<<"draw x="<<x<<" y="<<y<<std::endl;
        impl->draw(x, y);
    }
private:
   DrawImpl *impl;
};

template<typename T>
class TestTempl {
public:
    TestTempl()
    :t(new T)
    {

    }

    TestTempl(T& t1)
    {
        t = t1.t;
    }

    T& operator=(const T& t1)
    {
        t = t1.t;
        return *this;
    }

    //这边要用TestTempl   不能用T
    void swap(TestTempl& t1)
    {
        std::cout<<" draw templ swap "<<std::endl;
        using std::swap;
        t->swap(*t1.t);
    }

    void draw(const int& x, const int& y)
    {
        std::cout<<" templ draw "<<std::endl;
        t->draw(x,y);
    }

private:
    std::shared_ptr<T> t;
};

template<typename T>
void swap(TestTempl<T>& t1, 
          TestTempl<T>& t2)
{
    std::cout<<" function templ swap "<<std::endl;
    t1.swap(t2);
}

}
