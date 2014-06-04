#include "drawable.h"
#include <iostream>

static const double MINIMUM_INTERSECT_DISTANCE = 0.00001;


const glm::vec3 Drawable::blackColor = glm::vec3(0,0,0);
const glm::vec3 Drawable::whiteColor = glm::vec3(1,1,1);

/***            Drawable               ***/

Drawable::Drawable()
{

}


/***            Sphere               ***/

Sphere::Sphere(float radius) : Drawable()
{
    this->radius = radius;
}

Sphere::~Sphere()
{

}


bool Sphere::hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const
{
//    CVetor3D rayDirection( raio.getDirection() );
//    CVetor3D rayOrigin( raio.getStartPoint() );
//    CVetor3D sphereCenter( esfera.getCenterPosition() );
//    double sphereRadius = esfera.getRadius();
    double rayDirX = ray.x;
    double rayDirY = ray.y;
    double rayDirZ = ray.z;

    double rayOriginX = origin.x;
    double rayOriginY = origin.y;
    double rayOriginZ = origin.z;

    double sphereCenterX = position.x;
    double sphereCenterY = position.y;
    double sphereCenterZ = position.z;

    double A = rayDirX * rayDirX + rayDirY * rayDirY + rayDirZ * rayDirZ;
    double B = 2 * (rayDirX * (rayOriginX - sphereCenterX) + rayDirY * (rayOriginY - sphereCenterY) + rayDirZ * (rayOriginZ - sphereCenterZ));
    double C = (rayOriginX - sphereCenterX)*(rayOriginX - sphereCenterX) + (rayOriginY - sphereCenterY)*(rayOriginY - sphereCenterY) + (rayOriginZ - sphereCenterZ)*(rayOriginZ - sphereCenterZ) - radius*radius;

    double delta = B * B - 4 * A * C;
    if ( delta < 0 )
        return false;

    double root = sqrt( delta );
    double t1 = (-B + root) / (2 * A);
    double t2 = (-B - root) / (2 * A);

    if ( t1 < 0 && t2 < 0 )
        return false;

    if ( t2 < t1 )   // garante que t2 È sempre maior ou igual a t1 aqui
        std::swap( t1, t2 );

    double minT = t1;
    if ( t1 < 0 )
        minT = t2;
    else if ( t1 < MINIMUM_INTERSECT_DISTANCE )
        minT = t2;

    if ( minT < MINIMUM_INTERSECT_DISTANCE )
        return false;

//    intersectionPoint = CVetor3D(rayOrigin + rayDirection * minT ).getPointRepresentation();
    touchPoint = origin + (ray*(float)minT);
    return true;
}

glm::vec3 Sphere::getNormal(glm::vec3 point) const
{
    glm::vec3 normal =  point-position;

    return glm::normalize(normal);
}

/***            Plain               ***/

Plain::Plain()
{

}
