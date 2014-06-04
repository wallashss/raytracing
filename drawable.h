#pragma once

#include <QtCore>
#include <glm/glm.hpp>



class Drawable
{
public:
    Drawable();
    static const glm::vec3 whiteColor;
    static const glm::vec3 blackColor ;
public:
    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const =0;
    virtual glm::vec3 getNormal(glm::vec3 point) const = 0;
    glm::vec3 position;
    glm::vec3 color;
public:
    ~Drawable(){}

};

class Sphere : public Drawable
{

public:
    Sphere(float radius);

    ~Sphere();

    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const override;
    virtual glm::vec3 getNormal(glm::vec3 point) const override;
    float radius;
};

class Plain : public Drawable
{
public:
    Plain();
};
