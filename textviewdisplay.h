#ifndef TEXTVIEWDISPLAY_H
#define TEXTVIEWDISPLAY_H

#include <QWidget>

namespace Ui {
class TextViewDisplay;
}

class TextViewDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit TextViewDisplay(QWidget *parent = nullptr);
    ~TextViewDisplay();

private:
    Ui::TextViewDisplay *ui;
};

#endif // TEXTVIEWDISPLAY_H
