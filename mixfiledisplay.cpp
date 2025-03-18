#include "mixfiledisplay.h"
#include "ui_mixfiledisplay.h"
#include "dds.hpp"

#include <fmt/format.h>
#include <log4cxx/logger.h>
#include <log4cxx/hexdump.h>
#include <QImageReader>
#include <QImage>

static auto logger = log4cxx::Logger::getLogger("MIXFileDisplay");

struct BC1_Format{
    uint16_t color0;
    uint16_t color1;
    union{
        uint32_t color_idx;
        uint8_t color_tab[4];
    };
};

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
        bool loaded = loadDDS(fileName, *data);
        if(!loaded){
            LOG4CXX_DEBUG_FMT(logger, "Unable to load {} as DDS file", fileName.toStdString());
        }else{
            ui->tabWidget->setTabEnabled(2, true);
            ui->image_display->setPixmap(m_display_image);
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

bool MIXFileDisplay::loadDDS(QString fileName, std::span<const char> data){
    dds::Image img;
    dds::ReadResult res = dds::readImage((uint8_t*)(data.data()), data.size(), &img);
    if(res != dds::ReadResult::Success){
        return false;
    }

    LOG4CXX_DEBUG_FMT(logger, "Loaded {} as DDS.  {}x{} format:{} alpha? {}", fileName.toStdString(),
                      img.width, img.height,
                      (int)img.format,
                      img.supportsAlpha);

    // Now it's time to decode the raw data.
    switch(img.format){
    case DXGI_FORMAT_BC1_UNORM:
        decodeDDS_BC1(img.width, img.height, img.mipmaps[0]);
        break;
    default:
        return false;
    }

    return true;
}

static uint32_t rgb565_to_rgb888(uint16_t rgb565){
    uint8_t r = (rgb565 & (0x1F << 11)) >> 11;
    uint8_t g = (rgb565 & (0x3F << 5)) >> 5;
    uint8_t b = rgb565 & 0x1F;

    // https://stackoverflow.com/questions/2442576/how-does-one-convert-16-bit-rgb565-to-24-bit-rgb888
    r = ( r * 527 + 23 ) >> 6;
    g = ( g * 259 + 33 ) >> 6;
    b = ( b * 527 + 23 ) >> 6;

    r &= 0xFF;
    g &= 0xFF;
    b &= 0xFF;

    return (r << 16) | (g << 8) | b;
}

static std::array<uint32_t,2> linear_interp(uint32_t color0, uint32_t color1){
    uint32_t r1 = (color0 & (0xFF << 16)) >> 16;
    uint32_t g1 = (color0 & (0xFF << 8)) >> 8;
    uint32_t b1 = (color0 & (0xFF << 0)) >> 0;

    uint32_t r2 = (color1 & (0xFF << 16)) >> 16;
    uint32_t g2 = (color1 & (0xFF << 8)) >> 8;
    uint32_t b2 = (color1 & (0xFF << 0)) >> 0;

    uint32_t r_merge1 = r1 * .666 + r2 * .333;
    uint32_t g_merge1 = g1 * .666 + g2 * .333;
    uint32_t b_merge1 = b1 * .666 + b2 * .333;

    uint32_t r_merge2 = r1 * .333 + r2 * .666;
    uint32_t g_merge2 = g1 * .333 + r2 * .666;
    uint32_t b_merge2 = b1 * .333 + r2 * .666;

    r_merge1 &= 0xFF;
    r_merge2 &= 0xFF;
    g_merge1 &= 0xFF;
    g_merge2 &= 0xFF;
    b_merge1 &= 0xFF;
    b_merge2 &= 0xFF;

    return {
        (r_merge1 << 16) | (g_merge1 << 8) | b_merge1,
        (r_merge2 << 16) | (g_merge2 << 8) | b_merge2
    };
}

void MIXFileDisplay::decodeDDS_BC1(int width, int height, const std::span<uint8_t>& pixel_data){
    QImage tmp_image(width, height, QImage::Format_RGB32);
    BC1_Format* bc1_arr = (BC1_Format*)pixel_data.data();
    size_t num_array_entries = pixel_data.size() / sizeof(struct BC1_Format);
    int pixel_x_start = 0;
    int pixel_y_start = 0;

    LOG4CXX_DEBUG_FMT(logger, "Pixel data size: {} num entires: {}", pixel_data.size(), num_array_entries);

    for(size_t x = 0; x < num_array_entries; x++){
        std::array<uint32_t,2> interp = linear_interp(rgb565_to_rgb888(bc1_arr[x].color0),
                                                        rgb565_to_rgb888(bc1_arr[x].color1));

        std::array<uint32_t,4> color_values = {
            rgb565_to_rgb888(bc1_arr[x].color0),
            rgb565_to_rgb888(bc1_arr[x].color1),
            interp[0],
            interp[1]
        };

        // Now let's configure our 4x4 grid of pixels
        int curr_x = pixel_x_start;
        int curr_y = pixel_y_start;
        for(int color_tab_idx = 0; color_tab_idx < 4; color_tab_idx++){
            uint8_t color_tab = bc1_arr[x].color_tab[color_tab_idx];

            tmp_image.setPixel(curr_x + 3, curr_y, color_values[(color_tab & (0x3 << 6) ) >> 6 ]);
            tmp_image.setPixel(curr_x + 2, curr_y, color_values[(color_tab & (0x3 << 4) ) >> 4 ]);
            tmp_image.setPixel(curr_x + 1, curr_y, color_values[(color_tab & (0x3 << 2) ) >> 2 ]);
            tmp_image.setPixel(curr_x + 0, curr_y, color_values[(color_tab & (0x3 << 0) ) >> 0 ]);
            curr_y++;
        }

        pixel_x_start += 4;
        if(pixel_x_start >= width){
            pixel_x_start = 0;
            pixel_y_start += 4;
        }
    }

    m_display_image.convertFromImage(tmp_image);
//    tmp_image.save("/tmp/img.jpg");
}
