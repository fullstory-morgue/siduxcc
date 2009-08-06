install_files( /share/icons/hicolor/16x16/actions FILES ncc-lan-up.png ncc-lan-down.png ncc-wlan-up.png ncc-wlan-down.png)


kde4_install_icons(${ICON_INSTALL_DIR})


install(FILES  kcm_siduxcc_network.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
install_files( /bin FILES nicinfo-plus )
