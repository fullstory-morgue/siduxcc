find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
 
if(NOT GETTEXT_MSGFMT_EXECUTABLE)
	message(
"------
                 NOTE: msgfmt not found. Translations will *not* be installed
------")
else(NOT GETTEXT_MSGFMT_EXECUTABLE)
 
        set(catalogname siduxcc)
 
	add_custom_target(translations ALL)
	
	file(GLOB PO_FILES
		./da..po
		./de.po
		./el.po
		./es.po
		./fr.po
		./hr.po
		./it.po
		./ja.po
		./nl.po
		./pl.po
		./pt.po
		./ro.po
		./ru.po
	)
	
	FOREACH(_poFile ${PO_FILES})
		GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
		SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
		ADD_CUSTOM_COMMAND(TARGET translations
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
			DEPENDS ${_poFile})
		INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES/ RENAME ${catalogname}.mo)
	ENDFOREACH(_poFile ${PO_FILES})
 
ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)

