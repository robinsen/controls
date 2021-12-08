#pragma once

template <typename T>
class SimpleT {
public:
    T add(T a, T b) {
        return a+b;
    }

    SimpleT<T> multiply(T a, T b);
};

//普通模特类
template<typename T> 
class Compare {
public:
    bool isEqual(const T& t1, const T& t2);
};

//特化   不再具有模板的意思，因为使用了具体类型  
template<>
class Compare<double> {
public:
    bool isEqual(const double& d1, const double& d2);
};

//特化  不再具有模板的意思，因为使用了具体类型
template<>
class Compare<float> {
public:
    bool isEqual(const float& f1, const float& f2);
};

//模板偏特化   是指提供另一份template定义式，而其本身仍为templatized；也就是说，针对template参数更进一步的条件限制所设计出来的一个特化版本。
template<typename T1, typename T2>
class TestTemplate {
public:
    TestTemplate(){
        std::cout<<" T1 T2 "<<std::endl;
    }
};

template<typename T1, typename T2>
class TestTemplate<T1*,T2*> {
public:
    TestTemplate(){
        std::cout<<" T1* T2* "<<std::endl;
    }
};

template<typename T1, typename T2>
class TestTemplate<const T1&, const T2&> {
public:
    TestTemplate(){
        std::cout<<"const T1 T2 "<<std::endl;
    }
};