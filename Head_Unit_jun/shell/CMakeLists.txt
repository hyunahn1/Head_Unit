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
    widgets/GearPanel.h
    widgets/GearPanel.cpp
    widgets/SplashScreen.h
    widgets/SplashScreen.cpp
    widgets/ReverseCameraWindow.h
    widgets/ReverseCameraWindow.cpp
    ${CMAKE_SOURCE_DIR}/resources/headunit.qrc
)

if(Qt5WaylandCompositor_FOUND AND Qt5OpenGL_FOUND)
    list(APPEND SHELL_SOURCES
        HUCompositor.h
        HUCompositor.cpp
        ModuleSurfaceWidget.h
        ModuleSurfaceWidget.cpp
        ClusterOutputWindow.h
        ClusterOutputWindow.cpp
    )
endif()

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

if(Qt5WaylandCompositor_FOUND AND Qt5OpenGL_FOUND)
    target_link_libraries(hu_shell PRIVATE
        Qt5::WaylandCompositor
        Qt5::OpenGL
    )
endif()

install(TARGETS hu_shell RUNTIME DESTINATION bin)
