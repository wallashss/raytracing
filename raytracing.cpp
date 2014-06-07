#include "raytracing.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include <thread>


#define MAX_DIST 10000.0f



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

    glm::vec3 outColor(0,0,0);

    glm::vec3 occludedPosition;

    for(Light light : lights)
    {
        bool isShadow = false;
        for(Drawable * d : drawables)
        {
            if(d->name == drawable->name)
            {
                continue;
            }

            if(d->hasIntercepted(light.position,position,occludedPosition))
            {
                if(glm::distance(light.position,occludedPosition)< glm::distance(light.position,position))
                {
                    isShadow = true;
                    break;
                }
            }
        }

        glm::vec3 lightDir = glm::normalize( light.position - position);
        float lightDistance = glm::length(lightDir);

        float quadraticAttenuation=1;
        float linearAttenuation =1;
        float constantAttenuation =1;
        float attenuation = 1.0 / (constantAttenuation
                                   + linearAttenuation * lightDistance
                                   + quadraticAttenuation * lightDistance * lightDistance);

         glm::vec3 ambientColor = glm::vec3(0.1,0.1,0.1) * light.color * diffuseColor;
        if(isShadow)
        {
            float occludedDistance =  glm::distance(occludedPosition,position);

            float constantShadowAttenuation = 1.0f;
            float linearShadowAttenuation =0.3f;
            float quadraticShadowAttenuation=0.3f;
            float shadowAttenuation = 1.0 / (constantShadowAttenuation
                                             + linearShadowAttenuation * occludedDistance
                                             + quadraticShadowAttenuation * occludedDistance * occludedDistance);


            shadowAttenuation = fmin(shadowAttenuation,1);
            glm::vec3 shadowColor = ambientColor * shadowAttenuation;

            shadowColor = glm::min(ambientColor,shadowColor);
            outColor +=  ambientColor - shadowColor;

        }
        else
        {

            float diff = fmax(0, glm::dot(normal,lightDir));

            glm::vec3 reflection = glm::normalize(glm::reflect(-lightDir,normal));

            float specular = fmax(0,glm::dot(reflection,glm::normalize(viewDir)));

            specular = powf(specular,50);

            outColor += ambientColor;
            outColor += diffuseColor*diff;
            outColor += light.color*specular*attenuation*drawable->specularColor;

        }



    }

    //    glm::vec3  outColor = glm::vec3(specular,specular,specular);

    outColor.x = outColor.x <=1 ? outColor.x : 1;
    outColor.y = outColor.y <=1 ? outColor.y : 1;
    outColor.z = outColor.z <=1 ? outColor.z : 1;

    //    outColor = drawable->color;

    return outColor;
}

double schlickApproximation(double n1, double n2, glm::vec3 incident, glm::vec3 normal)
{
    double r0 = (n1-n2)/(n1+n2);
    r0*=r0;
    double cosI = -glm::dot(normal,incident);
    double cosX =cosI;
    if(n1>n2)
    {
        const double n = n1/n2;
        const double sinT2 = n*n*(1.0 - cosI*cosI);
        if(sinT2 >1.0)
        {
            // Total Internal Reflection!
            return 1.0;
        }
        cosX=sqrt(1.0-sinT2);
    }
    const double x =1.0-cosX;
    return r0+(1.0-r0)*x*x*x*x*x;

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
        else if(closestDrawable->type ==  Drawable::Type::TRANSPARENT)
        {

            if(n <=0)
            {
                return newColor;
            }
            glm::vec3 normal = closestDrawable->getNormal(closestPoint);


            float n1 =1.0f; // Air...
            float n2 =closestDrawable->refractionIndice;

            float R = schlickApproximation(n1,n2,glm::normalize(direction),normal);
            float T = 1.0f-R;

            glm::vec3 reflectedRay = glm::reflect(direction,normal);
            glm::vec3 transmissedRay = glm::refract(direction,normal,n1/n2);


            glm::vec3 transmissionColor;
            glm::vec3 oppositeSideOrigin = touchPoint+ 10000.0f*transmissedRay;
            glm::vec3 oppositeTouchPoint;

            // TODO: Enter and out
            if(closestDrawable->hasIntercepted(-transmissedRay,oppositeSideOrigin,oppositeTouchPoint) && false)
            {
                glm::vec3 newNormal = closestDrawable->getNormal(oppositeTouchPoint);
                glm::vec3 newRefractedRay = glm::refract(transmissedRay,newNormal,n2/n1);
                glm::vec3 newReflexionRay = glm::reflect(transmissedRay,newNormal);

                float R1 = schlickApproximation(n2,n1,glm::normalize(newRefractedRay),newNormal);
                float T1 = 1.0f-R;

                glm::vec3 transmissionColor1=traceRay(oppositeTouchPoint,glm::normalize(newRefractedRay), n-1,closestDrawable);
                glm::vec3 reflexionColor1=traceRay(oppositeTouchPoint,glm::normalize(newReflexionRay), n-1,closestDrawable);
                transmissionColor = (R1*reflexionColor1)+(T1*transmissionColor1);
            }
            else
            {
                 transmissionColor=traceRay(oppositeTouchPoint,glm::normalize(transmissedRay), n-1,closestDrawable);
            }

            glm::vec3 reflexionColor =  traceRay(closestPoint,glm::normalize(reflectedRay), n-1,closestDrawable);

            return  ((R*reflexionColor)+(T*transmissionColor))*newColor;

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
    float * r_mem = new float[2*size*2*_height];
    float * g_mem = new float[2*size*2*_height];
    float * b_mem = new float[2*size*2*_height];
    for(int i=0;i<size*_height*2*2;i++)
    {
        r_mem[i] =-1.0f;
        g_mem[i] =-1.0f;
        b_mem[i] =-1.0f;
    }
    for(auto i=offset;i<size;i++)
    {
        for(auto j=0;j<_height;j++)
        {
            glm::vec3 ray =  createRay(i,j, _width, _height);
            glm::vec3 newColor = traceRay(_camera,ray,defaultRecusion);

            if(multisample)
            {
                glm::vec3 newColor1;
                glm::vec3 newColor2;
                glm::vec3 newColor3;
                glm::vec3 newColor4;

                int i0=i*2;
                int i1=i*2+1;

                int j0=j*2;
                int j1=j*2+1;

                int size0 = size*2;


                if(r_mem[j0*size0+i0]<0 || g_mem[j0*size0+i0]<0 || b_mem[j0*size0+i0] <0)
                {
                    glm::vec3 ray1 =  createRay(i-0.5f,j-0.5f, _width, _height);
                    newColor1 = traceRay(_camera,ray1,defaultRecusion);
                    r_mem[j0*size0+i0] = newColor1.r;
                    g_mem[j0*size0+i0] = newColor1.g;
                    b_mem[j0*size0+i0] = newColor1.b;

                }
                else
                {
                    newColor1.r=r_mem[j0*size0+i0] ;
                    newColor1.g=g_mem[j0*size0+i0] ;
                    newColor1.b=b_mem[j0*size0+i0] ;
                }
                if(r_mem[j1*size0+i1]<0 || g_mem[j1*size0+i1]<0 || b_mem[j1*size0+i1] <0)
                {
                    glm::vec3 ray2 =  createRay(i+0.5f,j+0.5f, _width, _height);
                    newColor2 = traceRay(_camera,ray2,defaultRecusion);
                    r_mem[j1*size0+i1] = newColor2.r;
                    g_mem[j1*size0+i1] = newColor2.g;
                    b_mem[j1*size0+i1] = newColor2.b;

                }
                else
                {
                    newColor2.r=r_mem[j1*size0+i1] ;
                    newColor2.g=g_mem[j1*size0+i1] ;
                    newColor2.b=b_mem[j1*size0+i1] ;
                }

                if(r_mem[j0*size0+i1]<0 || g_mem[j0*size0+i1]<0 || b_mem[j0*size0+i1] <0)
                {
                    glm::vec3 ray3 =  createRay(i-0.5f,j+0.5f, _width, _height);
                    newColor3 = traceRay(_camera,ray3,defaultRecusion);
                    r_mem[j0*size0+i1] = newColor3.r;
                    g_mem[j0*size0+i1] = newColor3.g;
                    b_mem[j0*size0+i1] = newColor3.b;

                }
                else
                {
                    newColor3.r=r_mem[j0*size0+i1] ;
                    newColor3.g=g_mem[j0*size0+i1] ;
                    newColor3.b=b_mem[j0*size0+i1] ;
                }

                if(r_mem[j1*size0+i0]<0 || g_mem[j1*size0+i0]<0 || b_mem[j1*size0+i0] <0)
                {
                    glm::vec3 ray4 =  createRay(i+0.5f,j-0.5f, _width, _height);
                    newColor4 = traceRay(_camera,ray4,defaultRecusion);
                    r_mem[j1*size0+i0] = newColor4.r;
                    g_mem[j1*size0+i0] = newColor4.g;
                    b_mem[j1*size0+i0] = newColor4.b;

                }
                else
                {
                    newColor4.r=r_mem[j1*size0+i0] ;
                    newColor4.g=g_mem[j1*size0+i0] ;
                    newColor4.b=b_mem[j1*size0+i0] ;
                }
                newColor = (newColor+newColor1+newColor2+newColor3+newColor4)/5.0f;
            }


            unsigned int rgb = vectorToRgb(newColor);
            this->setRGB(i,j,rgb);
        }
    }
    delete [] r_mem;
    delete [] g_mem;
    delete [] b_mem;


}

void Raytracing::perform()
{

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
