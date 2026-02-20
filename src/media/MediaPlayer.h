/**
 * @file MediaPlayer.h
 * @brief QMediaPlayer wrapper - TODO: implement playback
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QUrl>

class MediaPlayer : public QObject
{
    Q_OBJECT

public:
    explicit MediaPlayer(QObject *parent = nullptr);

    void play();
    void pause();
    void stop();
    void setSource(const QUrl &url);
    bool isPlaying() const { return m_playing; }

signals:
    void positionChanged(qint64 ms);
    void durationChanged(qint64 ms);
    void stateChanged(bool playing);

private:
    bool m_playing;
};

#endif // MEDIAPLAYER_H
