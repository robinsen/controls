#include <iostream>
#include <cstring>
#include "graphicsimpl.h"

GraphicsImpl::GraphicsImpl()
{

}

GraphicsImpl::~GraphicsImpl()
{

}

void GraphicsImpl::drawLine(const int& x1, const int& y1)
{
    std::cout<<"impl draw line int "<<std::endl;
}

void GraphicsImpl::drawRectangle(const int& x0, const int& y0, const int& x1, const int& y1)
{
    std::cout<<"impl draw rect int "<<std::endl;
}

void GraphicsImpl::drawRectangle(const float& x0, const float& y0, const int& x1, const int& y1)
{
    std::cout<<"impl draw rect float "<<std::endl;
}