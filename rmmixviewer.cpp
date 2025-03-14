#include "rmmixviewer.h"
#include "./ui_rmmixviewer.h"

RMMixViewer::RMMixViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RMMixViewer)
{
    ui->setupUi(this);
}

RMMixViewer::~RMMixViewer()
{
    delete ui;
}

