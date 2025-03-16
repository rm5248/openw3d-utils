#include "mixfiledisplay.h"
#include "ui_mixfiledisplay.h"

#include <log4cxx/hexdump.h>

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

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);

    std::filesystem::path path = fileName.toStdString();
    // Now let's show the relevant portion in the display area
    if(data &&
            (path.extension() == ".txt" || path.extension() == ".ini") &&
            isFileText(*data)){
        // Check to make sure that the data is in fact text data - for some reason,
        // the always.dat from renegade has some txt/ini files that have non-printable characters
        QString text = QString::fromUtf8(data->data(), data->size());
        ui->tabWidget->setTabEnabled(1, true);
        ui->text_view->clear();
        ui->text_view->insertPlainText(text);
    }

    // Always display the hex information
    QString hexdump = QString::fromStdString(log4cxx::hexdump(data->data(), data->size()));
    ui->hex_text_view->clear();
    ui->hex_text_view->insertPlainText(hexdump);
}

bool MIXFileDisplay::isFileText(std::span<const char> data){
    for(const char c : data){
        QChar qc(c);
        if(c != '\n' &&
                c != '\r' &&
                !qc.isPrint()){
            return false;
        }
    }

    return true;
}
