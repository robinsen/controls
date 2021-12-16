#pragma once

#include <memory>

class GraphicsImpl;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void swap(Graphics& t);

    void drawLine(const int& x1, const int& y1);

    void drawRectangle(const int& x0, const int& y0, const int& x1, const int& y1);
    void drawRectangle(const float& x0, const float& y0, const int& x1, const int& y1);

private:
    std::shared_ptr<GraphicsImpl> impl;
};

//在这个命名空间内实现一个Graphics的swap的特化版本
namespace std {

template<>
inline void swap<Graphics>(Graphics &t1, 
                           Graphics &t2)
{
    std::cout<<" my swap "<<std::endl;
    t1.swap(t2);
}

}

