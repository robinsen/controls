#include <iostream>
#include <cstring>
#include "graphics.h"
#include "graphicsimpl.h"

Graphics::Graphics()
    :impl(new GraphicsImpl)  
{

}

Graphics::~Graphics()
{

}

void Graphics::swap(Graphics& t)
{
    using std::swap;
    swap(impl, t.impl);
    std::cout<<" impl swap "<<std::endl;
}

void Graphics::drawLine(const int& x1, const int& y1)
{
    std::cout<<"graphics draw line"<<std::endl;
    impl->drawLine(x1,y1);
}

void Graphics::drawRectangle(const int& x0, const int& y0, const int& x1, const int& y1)
{
    std::cout<<"graphics draw rect int "<<std::endl;
    impl->drawRectangle(x0,y0,x1,y1);
}

void Graphics::drawRectangle(const float& x0, const float& y0, const int& x1, const int& y1)
{
    std::cout<<" draw rect float "<<std::endl;
    impl->drawRectangle(x0,y0,x1,y1);
}