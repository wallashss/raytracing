#include "drawable.h"
#include <iostream>

#include <glm/gtx/intersect.hpp>

static const double MINIMUM_INTERSECT_DISTANCE = 0.00001;
static const double MAX_INTERSECT_DISTANCE = 0.1f;


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

Plane::Plane() : Drawable()
{

}
Plane::~Plane()
{

}

bool Plane::hasIntercepted(glm::vec3 ray, glm::vec3 origin, glm::vec3 & touchPoint) const
{

    glm::vec3 n = getNormal(position);
    float denom = glm::dot(n, glm::normalize(ray));
    if (denom > 1e-6) {
        glm::vec3 p0l0 = position - origin;
        float d = glm::dot(p0l0, n) / denom;
        touchPoint = origin + (ray-origin) * d;

        return (d >= 0);
    }
    return false;

//    return glm::intersectRayTriangle(ray,origin,position,pointA,pointB,touchPoint);
    std::swap(ray,origin);

    // Line and plane intersection is given by
    // A =  B^-1 * C
    // where A = [t u v] ^t
    // if t is between [0,1], so we have an intersection

    double xa = origin.x;
    double ya = origin.y;
    double za = origin.z;

    double xb = ray.x;
    double yb = ray.y;
    double zb = ray.z;

    double x0 =  position.x;
    double y0 =  position.y;
    double z0 =  position.z;

    double x1 =  pointA.x;
    double y1 =  pointA.y;
    double z1 =  pointA.z;

    double x2 =  pointB.x;
    double y2 =  pointB.y;
    double z2 =  pointB.z;

    glm::vec3 A;
    glm::mat3 B;
    glm::vec3 C;

    B[0][0] =xa-xb;
    B[0][1] =x1-x0;
    B[0][2] =x2-x0;

    B[1][0] =ya-yb;
    B[1][1] =y1-y0;
    B[1][2] =y2-y0;

    B[2][0] =za-zb;
    B[2][1] =z1-z0;
    B[2][2] =z2-z0;

    C.x = xa-x0;
    C.y = ya-y0;
    C.z = za-z0;
//    B = glm::transpose(B);

    glm::mat3 B_inverse = glm::inverse(B);
    A = B_inverse * C;

    float t = A[0];
    float u = A[1];
    float v = A[2];

    touchPoint = origin + (ray-origin) * t;


    if(((u+v) <= 1.0f) && (u >=0 && u<= 1) && (v>=0 && v<=1))
    {
        return true;
    }
    return false;
    if(MAX_INTERSECT_DISTANCE < glm::distance(origin,touchPoint))
    {
        return false;
    }
    else
    {
        return true;
    }
}
glm::vec3 Plane::getNormal(glm::vec3 point) const
{
    return glm::normalize(glm::cross(pointA - position, pointB -position));
}
