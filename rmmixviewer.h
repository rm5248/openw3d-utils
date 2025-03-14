#ifndef RMMIXVIEWER_H
#define RMMIXVIEWER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class RMMixViewer; }
QT_END_NAMESPACE

class RMMixViewer : public QMainWindow
{
    Q_OBJECT

public:
    RMMixViewer(QWidget *parent = nullptr);
    ~RMMixViewer();

private:
    Ui::RMMixViewer *ui;
};
#endif // RMMIXVIEWER_H
