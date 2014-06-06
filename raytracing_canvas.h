#pragma once

#include <QMainWindow>
#include  "raytracing.h"

class RaytracingCanvas : public QMainWindow
{
    Q_OBJECT

public:
    explicit RaytracingCanvas(int width, int height, int numOfThreads, bool multisample, QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    ~RaytracingCanvas();

    void render();

private:
    Raytracing _raytracing;

};

