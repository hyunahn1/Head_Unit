add_executable(hu_module_ambient
    main.cpp
    service/AmbientService.h
    service/AmbientService.cpp
    ui/AmbientWindow.h
    ui/AmbientWindow.cpp
    ../common/widgets/GearPanel.h
    ../common/widgets/GearPanel.cpp
)

target_include_directories(hu_module_ambient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/widgets
)

target_link_libraries(hu_module_ambient PRIVATE
    hu_core
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

install(TARGETS hu_module_ambient RUNTIME DESTINATION bin)
