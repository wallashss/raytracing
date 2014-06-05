#pragma once

#include <QtCore>
#include <glm/glm.hpp>


// An abstract drawable
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

// A sphere is defined by its position and its radius
class Sphere : public Drawable
{

public:
    Sphere(float radius);
    ~Sphere();

    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const override;
    virtual glm::vec3 getNormal(glm::vec3 point) const override;
    float radius;
};

// A plane is defined by its position and 2 more points no colinears

class Plane : public Drawable
{
public:
    Plane();
    ~Plane();

    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const override;
    virtual glm::vec3 getNormal(glm::vec3 point) const override;
    glm::vec3 pointA;
    glm::vec3 pointB;

};
