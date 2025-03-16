#include "rmmixviewer.h"

#include <QApplication>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>

static auto logger = log4cxx::Logger::getLogger("rmmixviewer");


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    log4cxx::PatternLayoutPtr pattern = std::make_shared<log4cxx::PatternLayout>("[%d{yyyy-MM-dd HH:mm:ss}] %c %-5p - %m%n");
    log4cxx::BasicConfigurator::configure(pattern);

    LOG4CXX_INFO(logger, "Starting up");

    RMMixViewer w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    return a.exec();
}
