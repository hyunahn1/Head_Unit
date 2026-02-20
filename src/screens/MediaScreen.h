/**
 * @file MediaScreen.h
 * @brief Media tab - Now Playing, playlist
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef MEDIASCREEN_H
#define MEDIASCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class MediaPlayer;
class PlaylistModel;
class GearStateManager;
class GearPanel;

class MediaScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MediaScreen(GearStateManager *gearState, QWidget *parent = nullptr);

private:
    MediaPlayer *m_player;
    PlaylistModel *m_playlist;
    GearPanel *m_gearPanel;
    QLabel *m_placeholder;
};

#endif // MEDIASCREEN_H
