#include "raytracing_canvas.h"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool useMultisample = false;
    int numThreads =1;

    if(argc >=2)
    {
        numThreads = atoi(argv[1]);
    }

    if(argc >=3)
    {
        useMultisample = atoi(argv[2]) > 0;
    }

    RaytracingCanvas w(800,600,numThreads,useMultisample);
    w.setWindowTitle("Executing raytracing");
    w.show();
    w.render();
    return a.exec();
}
