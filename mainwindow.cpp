#include "mainwindow.h"
#include <QPainter>
#include "drawable.h"
#include "raytracing.h"

MainWindow::MainWindow(int width, int height, QWidget *parent) :
    QMainWindow(parent),
    _raytracing(width,height)
{
    Sphere *sphere = new Sphere(5,"lightBlueSphere");
    sphere->position =glm::vec3(5,0,50.0f);
    sphere->color = glm::vec3(0.5,0.5,1);

    Sphere *sphere2 = new Sphere(2,"purpleSphere");
    sphere2->position =glm::vec3(0,0,20);
    sphere2->color = glm::vec3(1,0,1);

    Sphere *sphere3 = new Sphere(5,"blueSphere");
    sphere3->position =glm::vec3(-10,0,25.0f);
    sphere3->color = glm::vec3(0,0,1);

    Plane * floor = new Plane("floor");

    floor->position = glm::vec3(0,-5,0);
    floor->pointA = glm::vec3(1,-5,0);
    floor->pointB = glm::vec3(1,-5,1);

    floor->color = glm::vec3(0,1,0);

    Plane * back = new Plane("back");

    back->position = glm::vec3(0,0,70);
    back->pointA = glm::vec3(1,0,70);
    back->pointB = glm::vec3(1,1,70);
    back->color = glm::vec3(.7f,0.7f,0.7f);

    Plane * rightWall = new Plane("rightWall");
    rightWall->position = glm::vec3(15,0,0);
    rightWall->pointA = glm::vec3(15,1,0);
    rightWall->pointB = glm::vec3(15,1,1);

    rightWall->color = glm::vec3(0.7f,0.7f,0.7f);

    Plane * leftWall = new Plane("leftWall");
    leftWall->position = glm::vec3(-15,0,0);
    leftWall->pointA = glm::vec3(-15,1,1);
    leftWall->pointB = glm::vec3(-15,1,0);

    leftWall->color = glm::vec3(0.7f,0.7f,0.7f);


    _raytracing.drawables.push_back(sphere);
    _raytracing.drawables.push_back(sphere2);
    _raytracing.drawables.push_back(sphere3);
    _raytracing.drawables.push_back(floor);
    _raytracing.drawables.push_back(back);
    _raytracing.drawables.push_back(leftWall);
    _raytracing.drawables.push_back(rightWall);

    setFixedSize(width,height);

    Light light;
    light.position = glm::vec3(0,20,20);
    light.color = glm::vec3(1,1,1);

    Light light2;
    light2.position = glm::vec3(-20,30,40);
    light2.color = glm::vec3(1,1,1);

    Light light3;
    light3.position = glm::vec3(10,0,40);
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
