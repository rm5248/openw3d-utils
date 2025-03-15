#include "textviewdisplay.h"
#include "ui_textviewdisplay.h"

TextViewDisplay::TextViewDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextViewDisplay)
{
    ui->setupUi(this);
}

TextViewDisplay::~TextViewDisplay()
{
    delete ui;
}
