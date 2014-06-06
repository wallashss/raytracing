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
    bool useMultisample;
    int numThreads;
    unsigned int getRGB(int x,int y);
    unsigned int defaultRecusion;

private:
    int _width;
    int _height;
    unsigned int * _buffer;
    glm::vec3 _camera;


    void setRGB(int x, int y, unsigned int color);
    void renderBlock(int offset, int size, bool multisample);
    glm::vec3 computeColor(glm::vec3 position, glm::vec3 viewDir, Drawable * drawable);
    glm::vec3 traceRay(glm::vec3 origin, glm::vec3 direction, int n =0, Drawable * ignoredDrawable = nullptr);

};

