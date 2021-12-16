#pragma once
#include <memory>

class GraphicsImpl {
public:
    GraphicsImpl();
    ~GraphicsImpl();

    void drawLine(const int& x1, const int& y1);

    void drawRectangle(const int& x0, const int& y0, const int& x1, const int& y1);
    void drawRectangle(const float& x0, const float& y0, const int& x1, const int& y1);
};