find_package(KDE4 REQUIRED)
include( KDE4Defaults )
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} )


set(starter-sources 
   main.cpp
   starter.cpp
)

kde4_add_ui_files(starter-sources 
   starter.ui
)

kde4_add_executable(siduxcc-kde4 ${starter-sources})
target_link_libraries(siduxcc-kde4  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS}  ${KDE4_KDESU_LIBS} )
install(TARGETS siduxcc-kde4  ${INSTALL_TARGETS_DEFAULT_ARGS}} )


add_subdirectory( images )

install(FILES  siduxcc-kde4.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

########### install files ###############



