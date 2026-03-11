add_executable(hu_module_navigation
    main.cpp
    service/NavigationService.h
    service/NavigationService.cpp
    ui/NavigationWindow.h
    ui/NavigationWindow.cpp
    ../common/widgets/GearPanel.h
    ../common/widgets/GearPanel.cpp
)

target_include_directories(hu_module_navigation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/widgets
)

target_link_libraries(hu_module_navigation PRIVATE
    hu_core
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

if(Qt6WebEngineWidgets_FOUND)
    target_link_libraries(hu_module_navigation PRIVATE Qt6::WebEngineWidgets)
endif()

install(TARGETS hu_module_navigation RUNTIME DESTINATION bin)
