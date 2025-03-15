#ifndef MIXFILEDISPLAY_H
#define MIXFILEDISPLAY_H

#include <QWidget>

#include "mixfile.h"

namespace Ui {
class MIXFileDisplay;
}

class MIXFileDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit MIXFileDisplay(QWidget *parent = nullptr);
    ~MIXFileDisplay();

    void displayFileFromMIX(QString fileName, const openw3d::FileInfoStruct_Mix1 file_info, std::optional<std::span<const char>> data);

private:
    Ui::MIXFileDisplay *ui;
};

#endif // MIXFILEDISPLAY_H
