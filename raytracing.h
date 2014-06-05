#pragma once

#include "drawable.h"
#include <vector>
#include "light.h"

class Raytracing
{

public:
    Raytracing(int width, int height);
    ~Raytracing();
    std::vector<Drawable*> drawables;
    std::vector<Light> lights;
    void perform();
    int getWidth();
    int getHeight();
    unsigned int getRGB(int x,int y);

private:
    int _width;
    int _height;
    unsigned int * _buffer;
    void setRGB(int x, int y, unsigned int color);
    glm::vec3 computeColor(glm::vec3 position, Drawable * drawable);
    glm::vec3 _camera;
};

