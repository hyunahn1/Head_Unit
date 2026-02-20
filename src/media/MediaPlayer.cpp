/**
 * @file MediaPlayer.cpp
 * @brief Stub - replace with QMediaPlayer/QAudioOutput
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "MediaPlayer.h"

MediaPlayer::MediaPlayer(QObject *parent)
    : QObject(parent)
    , m_playing(false)
{
}

void MediaPlayer::play()
{
    m_playing = true;
    emit stateChanged(true);
}

void MediaPlayer::pause()
{
    m_playing = false;
    emit stateChanged(false);
}

void MediaPlayer::stop()
{
    m_playing = false;
    emit stateChanged(false);
}

void MediaPlayer::setSource(const QUrl &)
{
}
