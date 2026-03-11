cmake_minimum_required(VERSION 3.16)
project(piracer_head_unit VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Qt6 ────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# ── 옵셔널 컴포넌트 ────────────────────────────────────────────────────
find_package(Qt6 OPTIONAL_COMPONENTS Multimedia)
if(Qt6Multimedia_FOUND)
    message(STATUS "Qt Multimedia found - real audio enabled")
    add_compile_definitions(HU_MULTIMEDIA_AVAILABLE)
else()
    message(STATUS "Qt Multimedia NOT found - stub audio")
endif()

find_package(Qt6 OPTIONAL_COMPONENTS WebEngineWidgets)
if(Qt6WebEngineWidgets_FOUND)
    message(STATUS "Qt WebEngineWidgets found - YouTube/Navigation WebView enabled")
    add_compile_definitions(HU_WEBENGINE_AVAILABLE)
else()
    message(STATUS "Qt WebEngineWidgets NOT found - stub screens only")
endif()

# ── 모든 실행파일을 한 디렉토리에 모아서 ModuleController가 찾을 수 있게 ──
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ── 빌드 순서: core 먼저, 나머지 병렬 ─────────────────────────────────
add_subdirectory(core)
add_subdirectory(shell)
add_subdirectory(modules/media)
add_subdirectory(modules/youtube)
add_subdirectory(modules/call)
add_subdirectory(modules/navigation)
add_subdirectory(modules/ambient)
add_subdirectory(modules/settings)

# ── 설치: 모두 bin/ 에 모아서 ModuleController가 찾을 수 있게 ──────────
install(DIRECTORY config/ DESTINATION bin/config FILES_MATCHING PATTERN "*.json")
install(DIRECTORY python/ DESTINATION bin/python FILES_MATCHING PATTERN "*.py")
# hu_motor_control.py: scripts/ 사용 (게임패드 → 스로틀/스티어링, 기어는 /tmp/piracer_drive_mode.json)
if(EXISTS "${CMAKE_SOURCE_DIR}/../scripts/hu_motor_control.py")
    install(FILES "${CMAKE_SOURCE_DIR}/../scripts/hu_motor_control.py" DESTINATION bin/scripts)
endif()
