find_package(KDE4 REQUIRED)
include( KDE4Defaults )
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})


set(kcm_siduxcc_systemservices-sources 
   systemservices.cpp
)


kde4_add_ui_files(kcm_siduxcc_systemservices-sources
   systemservices.ui
)


kde4_add_plugin(kcm_siduxcc_systemservices ${kcm_siduxcc_systemservices-sources})
target_link_libraries(kcm_siduxcc_systemservices ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS}  ${KDE4_KDESU_LIBS})
install(TARGETS kcm_siduxcc_systemservices  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcm_siduxcc_systemservices.desktop     DESTINATION  ${SERVICES_INSTALL_DIR} )


