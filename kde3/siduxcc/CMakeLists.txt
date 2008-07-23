# list the source file

add_subdirectory( icons )

include(/usr/share/cmake-2.6/Modules/FindKDE3.cmake)
include(/usr/share/cmake-2.6/Modules/KDE3Macros.cmake)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set(siduxcc-kde3_SRCS
	../common/process.cpp
	main.cpp
	console.cpp
	start.cpp
	display.cpp
	network.cpp
	services.cpp
	software.cpp
	internet.cpp
	kernel.cpp
	info.cpp
)


kde3_add_ui_files( siduxcc-kde3_SRCS
	consoledialog.ui
	startdialog.ui
	displaydialog.ui
	networkdialog.ui
	servicesdialog.ui
	softwaredialog.ui
	internetdialog.ui
	kerneldialog.ui
	infodialog.ui
)


# automoc, more to say ?
kde3_automoc(${siduxcc-kde3_SRCS})

# generate the dcop stuff
#kde3_add_dcop_skels(kpager_SRCS  kpagerIface.h )

# create the binary named kpager from the sources listed in kpager_SRCS
kde3_add_executable(siduxcc-kde3 ${siduxcc-kde3_SRCS} ${MOC_HDRS})

# and link it to qt, kdecore and kdeui
target_link_libraries(siduxcc-kde3 kdeui)



install_files( /bin FILES siduxcc-kde3 )
install_files( /share/applications/kde FILES siduxcc-kde3.desktop )
#install_files( /helloworld FILES fabian/fabian.txt )

