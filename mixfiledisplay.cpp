#include "mixfiledisplay.h"
#include "ui_mixfiledisplay.h"
#include "dds.hpp"

#include <fmt/format.h>
#include <log4cxx/logger.h>
#include <log4cxx/hexdump.h>
#include <QImageReader>
#include <QImage>

static auto logger = log4cxx::Logger::getLogger("MIXFileDisplay");

MIXFileDisplay::MIXFileDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MIXFileDisplay)
{
    ui->setupUi(this);

    for(QByteArray ba : QImageReader::supportedImageFormats()){
        if(ba.compare("tga") == 0){
            m_support_tga = true;
        }
        LOG4CXX_DEBUG_FMT(logger, "Supported format: {}", ba.toStdString());
    }
}

MIXFileDisplay::~MIXFileDisplay()
{
    delete ui;
}

void MIXFileDisplay::displayFileFromMIX(QString fileName, const openw3d::FileInfoStruct_Mix1 file_info, std::optional<std::span<const char>> data){
    QString label_text;

    if(!data){
        return;
    }

    label_text = QString("CRC: %1\nSize: %2")
            .arg(file_info.CRC)
            .arg(file_info.Size);

    ui->basicFileInfo->setText(label_text);

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);

    std::filesystem::path path = fileName.toStdString();
    // Now let's show the relevant portion in the display area
    if((path.extension() == ".txt" || path.extension() == ".ini") &&
            isFileText(*data)){
        // Check to make sure that the data is in fact text data - for some reason,
        // the always.dat from renegade has some txt/ini files that have non-printable characters
        QString text = QString::fromUtf8(data->data(), data->size());
        ui->tabWidget->setTabEnabled(1, true);
        ui->text_view->clear();
        ui->text_view->insertPlainText(text);
    }else if(path.extension() == ".tga"){
        bool loaded = m_display_image.loadFromData(reinterpret_cast<const unsigned char*>(data->data()), data->size());
        if(loaded){
            LOG4CXX_DEBUG_FMT(logger, "Loaded {} as TGA file", fileName.toStdString());
            ui->image_display->setPixmap(m_display_image);
            ui->tabWidget->setTabEnabled(2, true);
        }else{
            LOG4CXX_DEBUG_FMT(logger, "Unable to load {} as TGA file", fileName.toStdString());
        }
    }else if(path.extension() == ".dds"){
        dds::Image img;
        dds::ReadResult res = dds::readImage((uint8_t*)(data->data()), data->size(), &img);
        if(res != dds::ReadResult::Success){
            LOG4CXX_DEBUG_FMT(logger, "Unable to load {} as DDS file", fileName.toStdString());
        }else{
            LOG4CXX_DEBUG_FMT(logger, "Loaded {} as DDS.  {}x{} format:{}", fileName.toStdString(),
                              img.width, img.height,
                              img.format);
        }
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
