add_executable(hu_module_media
    main.cpp
    service/MediaService.h
    service/MediaService.cpp
    ui/MediaWindow.h
    ui/MediaWindow.cpp
    media/MediaPlayer.h
    media/MediaPlayer.cpp
    media/PlaylistModel.h
    media/PlaylistModel.cpp
    ../common/widgets/GearPanel.h
    ../common/widgets/GearPanel.cpp
)

target_include_directories(hu_module_media PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/media
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/widgets
)

target_link_libraries(hu_module_media PRIVATE
    hu_core
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

if(Qt6Multimedia_FOUND)
    target_link_libraries(hu_module_media PRIVATE Qt6::Multimedia)
endif()

install(TARGETS hu_module_media RUNTIME DESTINATION bin)
