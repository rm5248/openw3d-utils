#include "mixfiledisplay.h"
#include "ui_mixfiledisplay.h"

MIXFileDisplay::MIXFileDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MIXFileDisplay)
{
    ui->setupUi(this);
}

MIXFileDisplay::~MIXFileDisplay()
{
    delete ui;
}

void MIXFileDisplay::displayFileFromMIX(QString fileName, const openw3d::FileInfoStruct_Mix1 file_info, std::optional<std::span<const char>> data){
    QString label_text;

    label_text = QString("ID: %1\nSize: %2")
            .arg(file_info.CRC)
            .arg(file_info.Size);

    ui->basicFileInfo->setText(label_text);

    std::filesystem::path path = fileName.toStdString();
    // Now let's show the relevant portion in the display area
    if(data && (path.extension() == ".txt" || path.extension() == ".ini")){
        QString text = QString::fromUtf8(data->data(), data->size());
        ui->text->setText(text);
    }else{
        ui->text->setText("Can't view at the moment");
    }
}
