#ifndef MIXFILEDISPLAY_H
#define MIXFILEDISPLAY_H

#include <QWidget>
#include <QPixmap>
#include <QTemporaryFile>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <memory>

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

private slots:
    void on_play_pause_clicked();
    void mediaPlaybackPositionChanged(int64_t pos);
    void mediaPlayerDurationChanged(int64_t duration);
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);

private:
    bool isFileText(std::span<const char>);
    bool loadDDS(QString fileName, std::span<const char>);
    void decodeDDS_BC1(int width, int height, const std::span<uint8_t>& pixel_data);
    void extractMP3();

private:
    Ui::MIXFileDisplay *ui;
    bool m_support_tga = false;
    QPixmap m_display_image;
    QMediaPlayer m_mediaPlayer;
    QAudioOutput m_audioOutput;
    bool m_haveExtractedMP3 = false;
    std::unique_ptr<QTemporaryFile> m_tmpMP3;
    std::span<const char> m_currentData;
};

#endif // MIXFILEDISPLAY_H
