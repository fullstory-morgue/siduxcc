# list the source file

add_subdirectory( icons )
add_subdirectory( scripts )

include(/usr/share/cmake-2.6/Modules/FindKDE3.cmake)
include(/usr/share/cmake-2.6/Modules/KDE3Macros.cmake)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set( SRCS
	../common/process.cpp
	main.cpp
	systray.cpp
	legend.cpp
	unique.cpp
	hermes.cpp
)


kde3_add_ui_files( SRCS
	legendbase.ui
	hermesbase.ui
)


# automoc, more to say ?
kde3_automoc( ${SRCS} )

# generate the dcop stuff
#kde3_add_dcop_skels(kpager_SRCS  kpagerIface.h )

# create the binary named kpager from the sources listed in kpager_SRCS
kde3_add_executable(sidux-hermes-kde3 ${SRCS} ${MOC_HDRS} )

# and link it to qt, kdecore and kdeui
target_link_libraries(sidux-hermes-kde3 kdeui )


install_files( /bin FILES sidux-hermes-kde3)
install_files( /share/applications/kde FILES sidux-hermes-kde3.desktop )

