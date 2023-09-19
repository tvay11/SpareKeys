cmake_minimum_required(VERSION 3.20)
project(SpareKeys)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(SpareKeys main.cpp mainwindow.h mainwindow.cpp EditCredentialDialog.cpp EditCredentialDialog.h)
target_link_libraries(SpareKeys
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

