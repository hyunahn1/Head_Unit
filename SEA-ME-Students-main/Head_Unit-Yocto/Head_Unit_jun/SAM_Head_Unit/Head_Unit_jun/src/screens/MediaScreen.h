/**
 * @file MediaScreen.h
 * @brief Media tab - Now Playing card, playlist, playback controls
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef MEDIASCREEN_H
#define MEDIASCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListView>
#include <QPixmap>
#include <QColor>

class MediaPlayer;
class PlaylistModel;
class GearStateManager;
class GearPanel;

class MediaScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MediaScreen(GearStateManager *gearState, QWidget *parent = nullptr);

private slots:
    void onPlayPause();
    void onPrev();
    void onNext();
    void onPositionChanged(qint64 ms);
    void onDurationChanged(qint64 ms);
    void onStateChanged(bool playing);
    void onTrackFinished();
    void onPlaylistItemActivated(const QModelIndex &index);
    void onAddFolder();
    void onProgressSliderMoved(int value);
    void onVolumeChanged(int value);
    void onCoverArtChanged(const QPixmap &art);
    void onError(const QString &msg);

signals:
    // Emitted whenever the album art accent colour changes (for external use)
    void accentColorChanged(const QColor &color);

private:
    void    playTrack(int row);
    void    showPlaceholderArt();
    QString formatTime(qint64 ms) const;
    void    applyAccentColor(const QColor &color);
    static QColor extractVibrantColor(const QPixmap &pixmap);

    MediaPlayer   *m_player;
    PlaylistModel *m_playlist;
    GearPanel     *m_gearPanel;

    // Now Playing UI
    QLabel      *m_albumArt;
    QLabel      *m_trackTitle;
    QLabel      *m_artistName;
    QPushButton *m_prevBtn;
    QPushButton *m_playPauseBtn;
    QPushButton *m_nextBtn;
    QSlider     *m_progressSlider;
    QLabel      *m_posLabel;
    QLabel      *m_durLabel;
    QSlider     *m_volumeSlider;

    // Playlist UI
    QListView *m_playlistView;

    int    m_currentTrack = -1;
    bool   m_seeking      = false;
    QColor m_accentColor  { 0, 212, 170 };  // default teal #00D4AA
};

#endif // MEDIASCREEN_H
