cmake_minimum_required(VERSION 3.16)

project(Graphical_Phonebook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Graphical_Phonebook
  main.cpp
  MainWindow.hpp MainWindow.cpp
  Contact.hpp Contact.cpp
)

target_link_libraries(Graphical_Phonebook PRIVATE Qt6::Widgets)