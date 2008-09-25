PROJECT(siduxcc)
cmake_minimum_required(VERSION 2.2)


add_subdirectory( po )
add_subdirectory( icons )

add_subdirectory( cli )
add_subdirectory( kde3 )
add_subdirectory( gtk2 )


# install the icons
#kde3_install_icons( hicolor )
