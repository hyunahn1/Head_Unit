set(SHELL_SOURCES
    main.cpp
    ShellWindow.h
    ShellWindow.cpp
    widgets/TabBar.h
    widgets/TabBar.cpp
    widgets/StatusBar.h
    widgets/StatusBar.cpp
    widgets/GlowOverlay.h
    widgets/GlowOverlay.cpp
    widgets/SplashScreen.h
    widgets/SplashScreen.cpp
    widgets/GearPanel.h
    widgets/GearPanel.cpp
    widgets/ReverseCameraWindow.h
    widgets/ReverseCameraWindow.cpp

    # ── 모듈 UI (in-process, 서브프로세스 없음) ──────────────────────────
    ../modules/media/ui/MediaWindow.h
    ../modules/media/ui/MediaWindow.cpp
    ../modules/media/media/MediaPlayer.h
    ../modules/media/media/MediaPlayer.cpp
    ../modules/media/media/PlaylistModel.h
    ../modules/media/media/PlaylistModel.cpp

    ../modules/youtube/ui/YouTubeWindow.h
    ../modules/youtube/ui/YouTubeWindow.cpp

    ../modules/call/ui/CallWindow.h
    ../modules/call/ui/CallWindow.cpp

    ../modules/navigation/ui/NavigationWindow.h
    ../modules/navigation/ui/NavigationWindow.cpp

    ../modules/ambient/ui/AmbientWindow.h
    ../modules/ambient/ui/AmbientWindow.cpp

    ../modules/settings/ui/SettingsWindow.h
    ../modules/settings/ui/SettingsWindow.cpp
)

add_executable(hu_shell ${SHELL_SOURCES})

target_include_directories(hu_shell PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/media/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/media/media
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/youtube/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/call/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/navigation/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ambient/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/settings/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/common/widgets
)

target_link_libraries(hu_shell PRIVATE
    hu_core
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

if(Qt5Multimedia_FOUND)
    target_link_libraries(hu_shell PRIVATE Qt5::Multimedia)
endif()

if(Qt5WebEngineWidgets_FOUND)
    target_link_libraries(hu_shell PRIVATE Qt5::WebEngineWidgets)
endif()

install(TARGETS hu_shell RUNTIME DESTINATION bin)
