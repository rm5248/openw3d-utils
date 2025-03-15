#include "rmmixviewer.h"

#include <QApplication>
#include <iostream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RMMixViewer w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    return a.exec();
}
