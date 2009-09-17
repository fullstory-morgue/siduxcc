project=siduxcc 

rm -rf `find . -name "CMakeCache.txt"`
rm -rf `find . -name "*.cmake"`
rm -rf `find . -name "*.gmo"`
rm -rf `find . -name "moc_*"`
rm -rf `find . -name "*.moc"`
rm -rf `find . -name "*.files"`
rm -rf `find . -name "*_automoc.cpp"`
rm -rf `find . -name ".directory"`




rm -rf `find . -name "ui_*.h"`
rm -rf `find . -name Makefile`
rm -rf `find . -type d -name CMakeFiles`

rm -rf CMakeTmp
rm -rf obj-i486-linux-gnu
rm -rf debian/tmp
rm -rf debian/$project
rm -rf debian/files
rm -rf debian/stamp-makefile-build
rm -rf debian/*.debhelper.log
rm -rf debian/*.substvars
rm -rf lib
rm kde4/starter/siduxcc-kde4.shell
rm kde4/starter/siduxcc-kde4
rm -rf debian/siduxcc-kde4
rm -rf debian/siduxcc-common
rm -rf debian/siduxcc-gtk2
rm -rf debian/siduxcc.postinst.debhelper
rm -rf debian/siduxcc.postrm.debhelper






