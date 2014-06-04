#pragma once

#include "drawable.h"
#include <vector>

class Raytracing
{

public:
    Raytracing(int width, int height);
    ~Raytracing();
    std::vector<Drawable*> drawables;
    void perform();
    int getWidth();
    int getHeight();
    unsigned int getRGB(int x,int y);
    glm::vec3 lightPosition;

private:
    int _width;
    int _height;
    unsigned int * _buffer;
    void setRGB(int x, int y, unsigned int color);
    glm::vec3 computeLight(glm::vec3 position, Drawable * drawable);
    glm::vec3 _camera;
};

