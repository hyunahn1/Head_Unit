cmake_minimum_required(VERSION 3.16)
project(head_unit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt5/Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

# VSOMEIP (optional)
find_path(VSOMEIP_INCLUDE_DIR vsomeip/vsomeip.hpp)
find_library(VSOMEIP_LIBRARY NAMES vsomeip3)
if(VSOMEIP_INCLUDE_DIR AND VSOMEIP_LIBRARY)
    message(STATUS "VSOMEIP found - gear IPC enabled")
    add_compile_definitions(HU_HAS_VSOMEIP)
endif()

# Multimedia: optional
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Multimedia)
if(Qt${QT_VERSION_MAJOR}Multimedia_FOUND)
    message(STATUS "Qt Multimedia found - real audio playback enabled")
    add_compile_definitions(HU_MULTIMEDIA_AVAILABLE)
else()
    message(STATUS "Qt Multimedia NOT found - stub playback (UI only)")
endif()

# WebEngineWidgets: optional - enables YouTube/Navigation WebView
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS WebEngineWidgets)
if(Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND)
    message(STATUS "Qt WebEngineWidgets found - YouTube/Navigation WebView enabled")
    add_compile_definitions(HU_WEBENGINE_AVAILABLE)
else()
    message(STATUS "Qt WebEngineWidgets NOT found - stub screens only")
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_standard_project_setup()
endif()

# AGL is removed from modern macOS SDK. Qt's FindWrapOpenGL.cmake hardcodes
# -framework AGL as a fallback when AGL.framework is not found.
# Remove it from WrapOpenGL::WrapOpenGL directly.
if(APPLE AND TARGET WrapOpenGL::WrapOpenGL)
    get_target_property(_wrap_gl_libs WrapOpenGL::WrapOpenGL INTERFACE_LINK_LIBRARIES)
    if(_wrap_gl_libs)
        list(FILTER _wrap_gl_libs EXCLUDE REGEX ".*AGL.*")
        set_target_properties(WrapOpenGL::WrapOpenGL PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_wrap_gl_libs}"
        )
    endif()
endif()


# Sources
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/widgets/TabBar.cpp
    src/widgets/GearPanel.cpp
    src/widgets/StatusBar.cpp
    src/widgets/GlowOverlay.cpp
    src/widgets/ReverseCameraWindow.cpp
    src/widgets/SplashScreen.cpp
    src/screens/MediaScreen.cpp
)
list(APPEND SOURCES src/screens/YouTubeScreen.cpp)
list(APPEND SOURCES
    src/screens/CallScreen.cpp
    src/screens/NavigationScreen.cpp
    src/screens/AmbientScreen.cpp
    src/screens/SettingsScreen.cpp
    src/ipc/MockVehicleDataProvider.cpp
    src/ipc/GearStateManager.cpp
    src/ipc/VSomeIPClient.cpp
    src/media/MediaPlayer.cpp
    src/media/PlaylistModel.cpp
    src/led/MockLedController.cpp
)

set(HEADERS
    src/MainWindow.h
    src/widgets/TabBar.h
    src/widgets/GearPanel.h
    src/widgets/StatusBar.h
    src/widgets/ReverseCameraWindow.h
    src/widgets/GlowOverlay.h
    src/widgets/SplashScreen.h
    src/screens/MediaScreen.h
)
list(APPEND HEADERS src/screens/YouTubeScreen.h)
list(APPEND HEADERS
    src/screens/CallScreen.h
    src/screens/NavigationScreen.h
    src/screens/AmbientScreen.h
    src/screens/SettingsScreen.h
    src/ipc/IVehicleDataProvider.h
    src/ipc/MockVehicleDataProvider.h
    src/ipc/GearStateManager.h
    src/ipc/VSomeIPClient.h
    src/media/MediaPlayer.h
    src/media/PlaylistModel.h
    src/led/ILedController.h
    src/led/MockLedController.h
)

set(RESOURCES
    resources/headunit.qrc
)

# Executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
    )
endif()

set(_hu_link_libs
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)
if(Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND)
    list(APPEND _hu_link_libs Qt${QT_VERSION_MAJOR}::WebEngineWidgets)
endif()
if(Qt${QT_VERSION_MAJOR}Multimedia_FOUND)
    list(APPEND _hu_link_libs Qt${QT_VERSION_MAJOR}::Multimedia)
endif()
if(VSOMEIP_INCLUDE_DIR AND VSOMEIP_LIBRARY)
    list(APPEND _hu_link_libs ${VSOMEIP_LIBRARY})
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ${_hu_link_libs})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screens
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media
    ${CMAKE_CURRENT_SOURCE_DIR}/src/led
)
if(VSOMEIP_INCLUDE_DIR AND VSOMEIP_LIBRARY)
    target_include_directories(${PROJECT_NAME} PRIVATE ${VSOMEIP_INCLUDE_DIR})
endif()

if(HU_PRE_API)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HU_PRE_API=1)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .  # Required for MACOSX_BUNDLE on Apple
)
install(DIRECTORY config/ DESTINATION bin/config FILES_MATCHING PATTERN "*.json")
install(DIRECTORY python/ DESTINATION bin/python FILES_MATCHING PATTERN "*.py")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
