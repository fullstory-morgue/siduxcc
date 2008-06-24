
TEMPLATE = app
CONFIG += qt

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
#UI_DIR = ui/


#FORMS =
#TRANSLATIONS = sidux-hermes-kde4.ts

#HEADERS += 
SOURCES += main.cpp \
 systray.cpp \
DESTDIR = .

target.path = /usr/bin
#iconstarget.path = /usr/share/siduxcc/icons
#iconstarget.files = res/*.png res/siduxcc.svg
#langtarget.path = /usr/share/siduxcc/lang
#langtarget.files = siduxcc_*.qm
#shortcuttarget.path = /usr/share/applications
#shortcuttarget.files = qbat.desktop

INSTALLS += target #iconstarget langtarget shortcuttarget

HEADERS += systray.h 
TARGET = sidux-hermes-kde4

#FORMS += settingsdialog.ui

