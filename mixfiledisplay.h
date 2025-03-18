#ifndef MIXFILEDISPLAY_H
#define MIXFILEDISPLAY_H

#include <QWidget>
#include <QPixmap>

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
    bool isFileText(std::span<const char>);
    bool loadDDS(QString fileName, std::span<const char>);
    void decodeDDS_BC1(int width, int height, const std::span<uint8_t>& pixel_data);

private:
    Ui::MIXFileDisplay *ui;
    bool m_support_tga = false;
    QPixmap m_display_image;
};

#endif // MIXFILEDISPLAY_H
