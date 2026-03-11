set(SHELL_SOURCES
    main.cpp
    ShellWindow.h
    ShellWindow.cpp
    ModuleController.h
    ModuleController.cpp
    ModuleBridge.h
    ModuleBridge.cpp
    widgets/TabBar.h
    widgets/TabBar.cpp
    widgets/StatusBar.h
    widgets/StatusBar.cpp
    widgets/GlowOverlay.h
    widgets/GlowOverlay.cpp
    widgets/SplashScreen.h
    widgets/SplashScreen.cpp
    widgets/FallbackContentWidget.h
    widgets/FallbackContentWidget.cpp
    widgets/GearPanel.h
    widgets/GearPanel.cpp
    widgets/ReverseCameraWindow.h
    widgets/ReverseCameraWindow.cpp
)

add_executable(hu_shell ${SHELL_SOURCES})

target_include_directories(hu_shell PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

target_link_libraries(hu_shell PRIVATE
    hu_core
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

install(TARGETS hu_shell RUNTIME DESTINATION bin)
