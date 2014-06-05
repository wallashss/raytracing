#include "mainwindow.h"
#include <QPainter>
#include "drawable.h"
#include "raytracing.h"

MainWindow::MainWindow(int width, int height, QWidget *parent) :
    QMainWindow(parent),
    _raytracing(width,height)
{
    Sphere *sphere = new Sphere(5);
    sphere->position =glm::vec3(5,-5,50.0f);
    sphere->color = glm::vec3(0.5,0.5,1);

    Sphere *sphere2 = new Sphere(10);
    sphere2->position =glm::vec3(0,5,100.0f);
    sphere2->color = glm::vec3(1,0,1);

    Sphere *sphere3 = new Sphere(5);
    sphere3->position =glm::vec3(-10,0,45.0f);
    sphere3->color = glm::vec3(0,0,1);

    Plane * floor = new Plane();

    floor->position = glm::vec3(0,-2,0);
    floor->pointA = glm::vec3(1,-2,0);
    floor->pointB = glm::vec3(1,-2,1);

    floor->color = glm::vec3(0,1,0);

    Plane * back = new Plane();

    back->position = glm::vec3(0,0,60);
    back->pointA = glm::vec3(1,0,60);
    back->pointB = glm::vec3(1,1,60);
    back->color = glm::vec3(.2f,0.2f,0.2f);

    Plane * rightWall = new Plane();
    rightWall->position = glm::vec3(5,0,0);
    rightWall->pointA = glm::vec3(5,1,0);
    rightWall->pointB = glm::vec3(5,0,1);

    rightWall->color = glm::vec3(0.5f,0.5f,0.5f);

    Plane * leftWall = new Plane();
    leftWall->position = glm::vec3(-5,0,0);
    leftWall->pointA = glm::vec3(-5,1,0);
    leftWall->pointB = glm::vec3(-5,0,1);

    leftWall->color = glm::vec3(0.5f,0.5f,0.5f);


//    _raytracing.drawables.push_back(sphere);
    _raytracing.drawables.push_back(sphere2);
//    _raytracing.drawables.push_back(sphere3);
    _raytracing.drawables.push_back(floor);
//    _raytracing.drawables.push_back(back);
//    _raytracing.drawables.push_back(rightWall);
    setFixedSize(width,height);

    Light light;
    light.position = glm::vec3(10,30,25);
    light.color = glm::vec3(1,1,1);

    Light light2;
    light2.position = glm::vec3(-20,30,40);
    light2.color = glm::vec3(1,1,1);

    Light light3;
    light3.position = glm::vec3(-10,-30,40);
    light3.color = glm::vec3(0.5,0.5,0.5);

    _raytracing.lights.push_back(light);
    _raytracing.lights.push_back(light2);
    _raytracing.lights.push_back(light3);
    _raytracing.perform();
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent *)
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
