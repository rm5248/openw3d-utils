#include "rmmixviewer.h"

#include <QApplication>
#include <log4cxx/basicconfigurator.h>

static auto logger = log4cxx::Logger::getLogger("MyApp");


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    log4cxx::BasicConfigurator::configure();

    LOG4CXX_ERROR(logger, "Starting up");

    RMMixViewer w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    return a.exec();
}
