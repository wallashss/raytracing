#include "raytracing.h"

#include <iostream>
#include <cmath>
static glm::vec3 createRay(int x, int y, int w, int h, glm::vec3 cameraPosition)
{
    glm::vec3 pixelPos(x-w/2,h/2-y,1000);
//    glm::vec3 dir = pixelPos - cameraPosition;
    return pixelPos;
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

    _camera = glm::vec3(0,0,-5.0f);
    for(int i=0;i<size;i++)
    {
        _buffer[i] = 0xFF000000; // Black
    }


}

Raytracing::~Raytracing()
{
    if (_buffer)
    {
        delete [] _buffer;
    }
}

glm::vec3 Raytracing::computeLight(glm::vec3 position, Drawable * drawable)
{
    glm::vec3 normal = drawable->getNormal(position);
    glm::vec3 color =drawable->color;
    glm::vec3 lightDir = glm::normalize( lightPosition - position);
    glm::vec3 ambientColor(0.01f,0.01f,0.01f);

    float diff = fmax(0, glm::dot(normal,lightDir));

    glm::vec3 reflection = glm::normalize(glm::reflect(-lightDir,normal));


    float specular = fmax(0,glm::dot(reflection,_camera));



    float lightDistance = glm::length(lightDir);

    float quadraticAttenuation=10;
    float linearAttenuation =10;
    float constantAttenuation =1;

    float attenuation = 1.0 / (constantAttenuation
                                  + linearAttenuation * lightDistance
                                  + quadraticAttenuation * lightDistance * lightDistance);

    glm::vec3 outColor = ambientColor + color*diff+ glm::vec3(specular,specular,specular)*attenuation;


    outColor.x = outColor.x <=1 ? outColor.x : 1;
    outColor.y = outColor.y <=1 ? outColor.y : 1;
    outColor.z = outColor.z <=1 ? outColor.z : 1;

//    outColor = glm::vec3(specular,specular,specular);

//    outColor
//     color * diff + glm::vec3(specular,specular,specular)
    return outColor;

//    return glm::vec3(diff,diff,diff);

}

void Raytracing::perform()
{
    int touchs = 0;
    std::vector<std::pair< std::pair<int,int>,glm::vec3>> zBuffer;
    for(auto i=0;i<_width;i++)
    {
        for(auto j=0;j<_height;j++)
        {

            glm::vec3 ray =  createRay(i,j, _width, _height, _camera);
            ray =glm::normalize(ray);

            glm::vec3 touchPoint;

            bool hasIntecepted = false;
            Drawable *closestDrawable;
            glm::vec3 closestPoint;


            for(auto it= drawables.begin();it!= drawables.end();++it)
            {
                Drawable * d = *it;


                if(d->hasIntercepted(ray, _camera, touchPoint))
                {
                    if(hasIntecepted)
                    {
                        float dist1 = glm::distance(closestPoint,_camera);
                        float dist2 = glm::distance(touchPoint,_camera);
                        if(dist1 > dist2)
                        {
                            closestPoint = touchPoint;
                            closestDrawable = d;
                        }
                    }
                    else
                    {
                        hasIntecepted = true;
                        closestPoint = touchPoint;
                        closestDrawable = d;
                    }
//                    std::cout << touchPoint.x << " " << touchPoint.y << " " << touchPoint.z << std::endl;

                }
            }
            if(hasIntecepted)
            {
                glm::vec3 newColor = computeLight(closestPoint,closestDrawable);
                unsigned int rgb = vectorToRgb(newColor);
                this->setRGB(i,j,rgb);
                touchs++;
                zBuffer.push_back({ {i,j},closestPoint});
            }

        }
    }

//    float maxDist =0;
//    float minDist = 0xFFFFFFFF;
//    for(auto it = points.begin();it!=points.end();++it)
//    {
//        std::pair< std::pair<int,int>,glm::vec3> aPair = *it;
//        std::pair<int,int> coord = aPair.first;
//        glm::vec3 point = aPair.second;

//        if(maxDist < glm::distance(_camera,point))
//        {
//            maxDist = glm::distance(_camera,point);
//        }
//        if(minDist > glm::distance(_camera,point))
//        {
//            minDist = glm::distance(_camera,point);
//        }
//    }

//    for(auto it = points.begin();it!=points.end();++it)
//    {
//        std::pair< std::pair<int,int>,glm::vec3> aPair = *it;
//        std::pair<int,int> coord = aPair.first;
//        glm::vec3 point = aPair.second;
//        float dist = glm::distance(_camera,point);
//        dist = (dist-minDist)/ maxDist;
//        unsigned int rgb = vectorToRgb(glm::vec3(dist,dist,dist));
//        this->setRGB(coord.first,coord.second,rgb);
//    }

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
