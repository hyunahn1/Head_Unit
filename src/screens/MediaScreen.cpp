/**
 * @file MediaScreen.cpp
 * @brief Placeholder - implement per HEAD_UNIT_DESIGN.md
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "MediaScreen.h"
#include "MediaPlayer.h"
#include "PlaylistModel.h"
#include "GearPanel.h"
#include "GearStateManager.h"

#include <QHBoxLayout>

MediaScreen::MediaScreen(GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_player(nullptr)
    , m_playlist(nullptr)
    , m_gearPanel(nullptr)
{
    setStyleSheet("background-color: #0D0D0F; color: #B3B3B7;");

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(24, 24, 0, 24);
    root->setSpacing(0);

    QVBoxLayout *content = new QVBoxLayout();
    m_placeholder = new QLabel("Media — Now Playing, Playlist\n(Implement per HEAD_UNIT_DESIGN.md)", this);
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet("font-size: 18pt; color: #6E6E73;");
    content->addWidget(m_placeholder, 1);

    root->addLayout(content, 1);
    m_gearPanel = new GearPanel(gearState, this);
    root->addWidget(m_gearPanel);

    // TODO: Add MediaPlayer, NowPlayingCard, PlaylistModel view
}
