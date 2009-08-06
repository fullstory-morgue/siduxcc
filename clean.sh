project=siduxcc 

rm -rf `find . -name "CMakeCache.txt"`
rm -rf `find . -name "*.cmake"`
rm -rf `find . -name "moc_*"`
rm -rf `find . -name "*.moc"`
rm -rf `find . -name "*.files"`
rm -rf `find . -name Makefile`
rm -rf `find . -type d -name CMakeFiles`

rm -rf CMakeTmp
rm -rf obj-i486-linux-gnu
rm -rf debian/tmp
rm -rf debian/$project
rm -rf debian/compat
rm -rf debian/files
rm -rf debian/stamp-makefile-build
rm -rf debian/*.debhelper.log
rm -rf debian/*.substvars




