#include "mainwindow.h"
#include <QPainter>
#include "drawable.h"
#include "raytracing.h"

MainWindow::MainWindow(int width, int height, QWidget *parent) :
    QMainWindow(parent),
    _raytracing(width,height)
{
    Sphere *sphere = new Sphere(5);
    sphere->position =glm::vec3(5,0,50.0f);
    sphere->color = glm::vec3(0.5,0.5,1);


    Sphere *sphere2 = new Sphere(25);
    sphere2->position =glm::vec3(0,0,100.0f);
    sphere2->color = glm::vec3(1,0,1);

    _raytracing.drawables.push_back(sphere);
    _raytracing.drawables.push_back(sphere2);
    setFixedSize(width,height);

    _raytracing.lightPosition = glm::vec3(10,30,25);
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
