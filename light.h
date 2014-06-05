#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

class Light
{

public:
    Light();
    glm::vec3 position;
    glm::vec3 color;
};

#endif // LIGHT_H
