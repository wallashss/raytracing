#pragma once

#include <QMainWindow>
#include  "raytracing.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int width, int height, QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    ~MainWindow();

private:
    Raytracing _raytracing;

};

