# list the source file

include(/usr/share/cmake-2.6/Modules/FindKDE3.cmake)
include(/usr/share/cmake-2.6/Modules/KDE3Macros.cmake)
include_directories( "/usr/include/kde/" "/usr/include/qt3/" ${CMAKE_CURRENT_SOURCE_DIR}/common )

add_subdirectory( siduxcc )
add_subdirectory( hermes )