#include "raytracing.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include <thread>


#define MAX_DIST 5000.0f

#include <chrono> // C++ 11

static glm::vec3 createRay(float x, float y, int w, int h)
{
    glm::vec3 pixelPos(x-w/2,h/2-y,1000);
    return glm::normalize(pixelPos);
}

static inline int vectorToRgb(glm::vec3 color)
{
    int r = color.r * 255;
    int g = color.g * 255;
    int b = color.b * 255;

    int rgb =0;
    r = r << 16;
    g = g << 8;

    rgb = r | g | b;

    return rgb;
}



Raytracing::Raytracing(int width, int height)
{
    this->_width = width;
    this->_height=height;

    int size =width*height;
    _buffer = new unsigned int[size];

    _camera = glm::vec3(0,0,-20.0f);
    for(int i=0;i<size;i++)
    {
        _buffer[i] = 0xFF000000; // Black
    }
    numThreads = 1;
    useMultisample = false;
    defaultRecusion = 0;
}

Raytracing::~Raytracing()
{
    if (_buffer)
    {
        delete [] _buffer;
    }
}

glm::vec3 Raytracing::computeColor(glm::vec3 position, glm::vec3 viewDir, Drawable * drawable)
{
    glm::vec3 normal = drawable->getNormal(position);
    glm::vec3 diffuseColor =drawable->getDiffuseColor(position);

    glm::vec3 outColor;

    for(Light light : lights)
    {
        bool isShadow = false;
        for(Drawable * d : drawables)
        {
            glm::vec3 i;
            if(d->name == drawable->name)
            {
                continue;
            }

            if(d->hasIntercepted(light.position,position,i))
            {
                if(glm::distance(light.position,i)< glm::distance(light.position,position))
                {
                    isShadow = true;
                    break;
                }

            }
        }
        if(isShadow)
        {
            continue;
        }

        glm::vec3 lightDir = glm::normalize( light.position - position);
        glm::vec3 ambientColor = glm::vec3(0.1,0.1,0.1) * light.color * diffuseColor;
//        glm::vec3 viewDir = _camera - position;

        float diff = fmax(0, glm::dot(normal,lightDir));

        glm::vec3 reflection = glm::normalize(glm::reflect(-lightDir,normal));

        float specular = fmax(0,glm::dot(reflection,glm::normalize(viewDir)));

        specular = powf(specular,50);

        float lightDistance = glm::length(lightDir);

        float quadraticAttenuation=1;
        float linearAttenuation =1;
        float constantAttenuation =1;

        float attenuation = 1.0 / (constantAttenuation
                                      + linearAttenuation * lightDistance
                                      + quadraticAttenuation * lightDistance * lightDistance);

        outColor += ambientColor;
        outColor += diffuseColor*diff;
        outColor += light.color*specular*attenuation*drawable->specularColor;

    }

//    glm::vec3  outColor = glm::vec3(specular,specular,specular);

    outColor.x = outColor.x <=1 ? outColor.x : 1;
    outColor.y = outColor.y <=1 ? outColor.y : 1;
    outColor.z = outColor.z <=1 ? outColor.z : 1;

//    outColor = drawable->color;

    return outColor;

//    return glm::vec3(diff,diff,diff);

}

glm::vec3 Raytracing::traceRay(glm::vec3 origin, glm::vec3 direction, int n, Drawable * ignoredDrawable)
{
    glm::vec3 touchPoint;

    bool hasIntecepted = false;
    Drawable *closestDrawable;
    glm::vec3 closestPoint;

    for(auto it= drawables.begin();it!= drawables.end();++it)
    {
        Drawable * d = *it;


        if(ignoredDrawable )
        {
            if(d->name == ignoredDrawable->name)
            {
                continue;
            }
        }

        if(d->hasIntercepted(direction, origin, touchPoint))
        {
            if(hasIntecepted)
            {
                double dist1 = glm::distance(closestPoint,origin);
                double dist2 = glm::distance(touchPoint,origin);
                if(dist1 > dist2)
                {
                    if (dist1 < MAX_DIST)
                    {
                        closestPoint = touchPoint;
                        closestDrawable = d;
                    }

                }
            }
            else
            {
                double dist = glm::distance(touchPoint,origin);
                if(dist<MAX_DIST)
                {
                    hasIntecepted = true;
                    closestPoint = touchPoint;
                    closestDrawable = d;
                }
            }

        }
    }
    if(hasIntecepted)
    {

        glm::vec3 newColor = computeColor(closestPoint,origin-closestPoint, closestDrawable);
        if(closestDrawable->type ==  Drawable::Type::REFLEXIVE)
        {
            if(n <= 0)
            {
                return newColor;
            }
            glm::vec3 normal = closestDrawable->getNormal(closestPoint);
            glm::vec3 newRay = glm::reflect(direction,normal);
            glm::vec3 reflexColor =traceRay(closestPoint,glm::normalize(newRay), n-1);
            return (reflexColor + newColor) /2.0f;
        }
        else if(closestDrawable->type ==  Drawable::Type::SEMI_TRANSPARENT)
        {
            if(n <=0)
            {
                return newColor;
            }
            glm::vec3 normal = closestDrawable->getNormal(closestPoint);
            glm::vec3 newRay = glm::refract(direction,normal,1/1.445f); // Air / Glass

            glm::vec3 refractionColor =traceRay(closestPoint,newRay, n-1,closestDrawable);
            return  refractionColor * newColor;

        }
        else
        {
            return newColor;
        }
    }
    return glm::vec3(0,0,0);

}

void Raytracing::renderBlock(int offset, int size, bool multisample)
{
    for(auto i=offset;i<size;i++)
    {
        for(auto j=0;j<_height;j++)
        {
            glm::vec3 ray =  createRay(i,j, _width, _height);
            glm::vec3 newColor = traceRay(_camera,ray,defaultRecusion);

            if(multisample)
            {
                glm::vec3 ray1 =  createRay(i-0.5f,j+0.5f, _width, _height);
                glm::vec3 newColor1 = traceRay(_camera,ray1,defaultRecusion);

                glm::vec3 ray2 =  createRay(i+0.5f,j+0.5f, _width, _height);
                glm::vec3 newColor2 = traceRay(_camera,ray2,defaultRecusion);

                glm::vec3 ray3 =  createRay(i+0.5f,j-0.5f, _width, _height);
                glm::vec3 newColor3 = traceRay(_camera,ray3,defaultRecusion);

                glm::vec3 ray4 =  createRay(i-0.5f,j-0.5f, _width, _height);
                glm::vec3 newColor4 = traceRay(_camera,ray4,defaultRecusion);

                newColor = (newColor+newColor1+newColor2+newColor3+newColor4)/5.0f;
            }


            unsigned int rgb = vectorToRgb(newColor);
            this->setRGB(i,j,rgb);
        }
    }


}

void Raytracing::perform()
{
    auto start = std::chrono::high_resolution_clock::now();

    int n = numThreads;
    int size = roundf((float)_width/n);

    std::cout << size << std::endl;

    std::vector<std::thread> threads;

    for(int i=0;i<n;i++)
    {

        if(i<n-1)
        {
            threads.push_back(
            std::thread([=]{
                renderBlock(i*size,i*size+size, useMultisample);
            }));
        }
        else
        {
            threads.push_back(
            std::thread([=]{
                renderBlock(i*size,_width, useMultisample);
            }));
        }
    }

    for(auto & thread : threads)
    {
        thread.join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Time to render: "<< std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()/1e+9 << " s\n";

}



int Raytracing::getWidth()
{
    return _width;
}

int Raytracing::getHeight()
{
    return _height;
}

unsigned int Raytracing::getRGB(int x, int y)
{
    return _buffer[y*_width+x];
}

void Raytracing::setRGB(int x, int y, unsigned int color)
{
    _buffer[y*_width+x] = color;
}
