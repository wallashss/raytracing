#pragma once

#include <QtCore>
#include <glm/glm.hpp>


// An abstract drawable
class Drawable
{
public:
    enum class Type{
        OPAQUE, REFLEXIVE, SEMI_TRANSPARENT
    };

    Drawable(std::string newName = "");
    std::string name;
    static const glm::vec3 whiteColor;
    static const glm::vec3 blackColor ;
    Type type;
public:
    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const =0;
    virtual glm::vec3 getNormal(glm::vec3 point) const = 0;
    virtual glm::vec3 getDiffuseColor(glm::vec3 point) const = 0;
    glm::vec3 position;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
public:
    ~Drawable(){}

};

// A sphere is defined by its position and its radius
class Sphere : public Drawable
{

public:
    Sphere(float radius, std::string newName = "");
    ~Sphere();

    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const override;
    virtual glm::vec3 getNormal(glm::vec3 point) const override;
    virtual glm::vec3 getDiffuseColor(glm::vec3 point) const override;
    float radius;
};

// A plane is defined by its position and 2 more points no colinears

class Plane : public Drawable
{
public:
    Plane(std::string newName = "");
    ~Plane();

    virtual bool hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const override;
    virtual glm::vec3 getNormal(glm::vec3 point) const override;
    virtual glm::vec3 getDiffuseColor(glm::vec3 point) const override;
    glm::vec3 pointA;
    glm::vec3 pointB;

};

class Chess : public Plane
{
public:
    Chess(std::string newName = "")  ;
    float tileSize;
    glm::vec3 getDiffuseColor(glm::vec3 point) const override;
    glm::vec3 color1;
    glm::vec3 color2;
};
