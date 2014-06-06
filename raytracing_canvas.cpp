#include "raytracing_canvas.h"
#include <QPainter>
#include "drawable.h"
#include "raytracing.h"
#include <chrono> // C++ 11
#include <iostream>
#include <sstream>

RaytracingCanvas::RaytracingCanvas(int width, int height,int numOfThreads, bool multisample, QWidget *parent) :
    QMainWindow(parent),
    _raytracing(width,height)
{
    _raytracing.numThreads = numOfThreads;
    _raytracing.useMultisample =multisample;

    Sphere *reflexiveSphere = new Sphere(5,"lightBlueSphere");
    reflexiveSphere->position =glm::vec3(5,0,18.0f);
    reflexiveSphere->diffuseColor = glm::vec3(0.5,0.5,1);
    reflexiveSphere->type = Drawable::Type::REFLEXIVE;

    Sphere *purpleSphere = new Sphere(2,"purpleSphere");
    purpleSphere->position =glm::vec3(-5,-3,20);

//     purpleSphere->position =glm::vec3(5,-3.0f,5.0f);
    purpleSphere->diffuseColor = glm::vec3(1,0,1);

    Sphere *blueSphere = new Sphere(5,"blueSphere");
    blueSphere->position =glm::vec3(-10,0,25.0f);
    blueSphere->diffuseColor = glm::vec3(0,0,1);

    Sphere *greenSphere = new Sphere(1,"greenSphere");
    greenSphere->position =glm::vec3(5,-4,4.0f);
    greenSphere->diffuseColor = glm::vec3(0,1,0);
    greenSphere->specularColor = glm::vec3(0,0,0);

    Sphere *redSphere = new Sphere(2,"redSphere");
    redSphere->position =glm::vec3(1,-3,5.0f);
    redSphere->diffuseColor = glm::vec3(1,0,0);
    redSphere->specularColor = glm::vec3(2,0,5);

    Sphere *transparentSphere = new Sphere(2,"transparentSphere");
    transparentSphere->position =glm::vec3(-5,-3,5.0f);

//    transparentSphere->position =glm::vec3(-5,2.0f,5.0f);
    transparentSphere->diffuseColor = glm::vec3(1,1,1);
    transparentSphere->specularColor = glm::vec3(1,1,1);

    transparentSphere->type = Drawable::Type::SEMI_TRANSPARENT;

    Sphere *yellowSphere = new Sphere(1,"yellowSphere");
    yellowSphere ->position =glm::vec3(-0,-4,1.0f);
    yellowSphere ->diffuseColor = glm::vec3(2,2,0);
    yellowSphere ->specularColor = glm::vec3(1,1,1);

    yellowSphere->type = Drawable::Type::SEMI_TRANSPARENT;


    Chess * floor = new Chess("floor");

    floor->position = glm::vec3(0,-5,0);
    floor->pointA = glm::vec3(1,-5,0);
    floor->pointB = glm::vec3(1,-5,1);

    floor->color1 = Drawable::blackColor;
    floor->color2 = glm::vec3(1.5f,1.5f,1.5f);
    floor->tileSize = 2.5f;

    floor->type = Drawable::Type::REFLEXIVE;

    Plane * ceil = new Plane("ceil");

    ceil->position = glm::vec3(0,20,0);
    ceil->pointA = glm::vec3(1,20,1);
    ceil->pointB = glm::vec3(1,20,0);
    ceil->diffuseColor = glm::vec3(0.6f,0.6f,0.6f);


    Plane * back = new Plane("back");

    back->position = glm::vec3(0,0,50);
    back->pointA = glm::vec3(1,0,50);
    back->pointB = glm::vec3(1,1,50);
    back->diffuseColor = glm::vec3(.2f,0.2f,0.2f);
    back->specularColor = glm::vec3(2.0f,2.0f,2.0f);
    back->type = Drawable::Type::REFLEXIVE;

    Plane * front = new Plane("front");

    front->position = glm::vec3(0,0,-10);
    front->pointA = glm::vec3(1,1,-10);
    front->pointB = glm::vec3(1,0,-10);
    front->diffuseColor = glm::vec3(.7f,0.7f,0.7f);

    Plane * rightWall = new Plane("rightWall");
    rightWall->position = glm::vec3(15,0,0);
    rightWall->pointA = glm::vec3(15,1,0);
    rightWall->pointB = glm::vec3(15,1,1);

    rightWall->diffuseColor = glm::vec3(1,0,0);


    Plane * leftWall = new Plane("leftWall");
    leftWall->position = glm::vec3(-15,0,0);
    leftWall->pointA = glm::vec3(-15,1,1);
    leftWall->pointB = glm::vec3(-15,1,0);

    leftWall->diffuseColor = glm::vec3(0,0,1);


    _raytracing.drawables.push_back(reflexiveSphere);
    _raytracing.drawables.push_back(purpleSphere);
    _raytracing.drawables.push_back(blueSphere);
    _raytracing.drawables.push_back(floor);
    _raytracing.drawables.push_back(back);
    _raytracing.drawables.push_back(front);
    _raytracing.drawables.push_back(ceil);
    _raytracing.drawables.push_back(leftWall);
    _raytracing.drawables.push_back(rightWall);
    _raytracing.drawables.push_back(greenSphere);
    _raytracing.drawables.push_back(redSphere);
    _raytracing.drawables.push_back(transparentSphere);
    _raytracing.drawables.push_back(yellowSphere);

    setFixedSize(width,height);

    Light light;
    light.position = glm::vec3(0,15,20);
    light.color = glm::vec3(1,1,1);

    Light light2;
    light2.position = glm::vec3(-10,15,40);
    light2.color = glm::vec3(1,1,1);

    Light light3;
    light3.position = glm::vec3(0,5,0);
    light3.color = glm::vec3(2,2,2);

    Light light4;
    light4.position = glm::vec3(0,10,0);
    light4.color = glm::vec3(1,1,1);

    _raytracing.lights.push_back(light);
    _raytracing.lights.push_back(light2);
    _raytracing.lights.push_back(light3);
    _raytracing.lights.push_back(light4);

    _raytracing.defaultRecusion = 5;

    resize(width,height);

}

RaytracingCanvas::~RaytracingCanvas()
{

}
void RaytracingCanvas::render()
{
    auto start = std::chrono::high_resolution_clock::now();
    _raytracing.perform();
    auto finish = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()/1e+9;
    std::cout << "Time to render: "<< timeElapsed << " s\n";
    std::stringstream stringStream;
    stringStream << "Time to render: " << timeElapsed << " s.";
    this->setWindowTitle(stringStream.str().c_str());
}

void RaytracingCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QColor(0,0,0));

    // set black canvas
    for(auto i = 0;i< width();i++)
    {
        for(auto j=0;j<height();j++)
        {
            painter.drawPoint(i,j);
        }
    }


    for(int i=0;i<_raytracing.getWidth();i++)
    {
        for(int j=0;j<_raytracing.getHeight();j++)
        {
            int rgb = _raytracing.getRGB(i,j);
            int r = 0x00FF0000 & rgb;
            int g = 0x0000FF00 & rgb;
            int b = 0x000000FF & rgb;

            // shift bits!
            r  = r >> 16;
            g = g >> 8;

            painter.setPen(QColor(r,g,b));
            painter.drawPoint(i,j);

        }
    }
}
