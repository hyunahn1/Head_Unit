/**
 * @file YouTubeScreen.h
 * @brief YouTube tab - WebEngine video playback
 */

#ifndef YOUTUBESCREEN_H
#define YOUTUBESCREEN_H

#include <QWidget>

class GearStateManager;
class GearPanel;

#ifdef HU_WEBENGINE_AVAILABLE
class QWebEngineView;
#endif

class YouTubeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit YouTubeScreen(GearStateManager *gearState, QWidget *parent = nullptr);

private:
    void setupUI(GearStateManager *gearState);

    GearPanel *m_gearPanel;
#ifdef HU_WEBENGINE_AVAILABLE
    QWebEngineView *m_webView;
#endif
};

#endif // YOUTUBESCREEN_H
