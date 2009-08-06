set( kcm_siduxcc_network-sources 
   netcardconfig.cpp
   lan.cpp
   wlan.cpp
   settings.cpp
)

kde4_add_ui_files( kcm_siduxcc_network-sources
   netcardconfig.ui
   lan.ui
   wlan.ui
)


kde4_add_plugin(kcm_siduxcc_network ${kcm_siduxcc_network-sources})
target_link_libraries(kcm_siduxcc_network ${KDE4_KDEUI_LIBS}  ${KDE4_KDESU_LIBS})
install(TARGETS kcm_siduxcc_network  DESTINATION ${PLUGIN_INSTALL_DIR} )