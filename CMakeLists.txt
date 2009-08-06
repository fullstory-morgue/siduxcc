PROJECT(siduxcc)
cmake_minimum_required(VERSION 2.2)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})


add_subdirectory( po )
add_subdirectory( icons )

add_subdirectory( cli )
add_subdirectory( kde4 )
add_subdirectory( gtk2 )
