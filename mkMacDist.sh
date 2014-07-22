#!/bin/bash
# Create a distributable installable package

DIST_DIR=scidavis/scidavis.app/Contents/MacOS
RES_DIR=scidavis/scidavis.app/Contents/Resources

# We to copy all dynamic libraries, and rewrite the absolute link paths to be 
# relative
for i in libqwt.5.dylib libqwtplot3d.0.dylib libmuparser.0.dylib QtAssistant.framework/Versions/4/QtAssistant libz.1.dylib libssl.1.0.0.dylib libcrypto.1.0.0.dylib libpng16.16.dylib; do 
    cp -f /opt/local/lib/$i $DIST_DIR
    install_name_tool -change /opt/local/lib/$i @executable_path/${i##*/} $DIST_DIR/scidavis
done

# and same for Framework dylibs
for i in Python.framework/Versions/2.7/Python QtCore.framework/Versions/4/QtCore QtNetwork.framework/Versions/4/QtNetwork QtSvg.framework/Versions/4/QtSvg Qt3Support.framework/Versions/4/Qt3Support QtSql.framework/Versions/4/QtSql QtXml.framework/Versions/4/QtXml QtOpenGL.framework/Versions/4/QtOpenGL QtGui.framework/Versions/4/QtGui; do
    cp -f /opt/local/Library/Frameworks/$i $DIST_DIR
    install_name_tool -change /opt/local/Library/Frameworks/$i @executable_path/${i##*/} $DIST_DIR/scidavis
done

chmod u+w $DIST_DIR/*

# finally rewrite references contained within the dylibs themselves
for i in Qt3Support.framework/Versions/4/Qt3Support QtGui.framework/Versions/4/QtGui QtCore.framework/Versions/4/QtCore QtSql.framework/Versions/4/QtSql QtXml.framework/Versions/4/QtXml QtNetwork.framework/Versions/4/QtNetwork QtOpenGL.framework/Versions/4/QtOpenGL; do
    for j in $DIST_DIR/*; do
        install_name_tool -change /opt/local/Library/Frameworks/$i @executable_path/${i##*/} $j
    done
done

for i in libz.1.dylib libssl.1.0.0.dylib libcrypto.1.0.0.dylib libpng16.16.dylib; do 
    for j in $DIST_DIR/*; do
        install_name_tool -change /opt/local/lib/$i @executable_path/${i##*/} $j
    done
done

# Generic resources required for Qt
cp -rf /opt/local/Library/Frameworks/QtGui.framework/Resources/qt_menu.nib $RES_DIR

# python resources
mkdir -p $RES_DIR/lib
cp -rf /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7 $RES_DIR/lib

# python resources contain some dynamic libraries that need rewriting
find $RES_DIR/lib -name "*.so" -print | while read soname; do
    otool -L $soname|grep /opt/local|cut -f1 -d' '|while read oldName; do
        install_name_tool -change $oldName @executable_path/${oldName##*/} $soname
        done
done



# copy icon, and create mainfest
cp -f scidavis/icons/scidavis.icns $RES_DIR
cat >scidavis/scidavis.app/Contents/Info.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN"
 "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>NSPrincipalClass</key>
	<string>NSApplication</string>
	<key>CFBundleIconFile</key>
	<string>scidavis.icns</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleGetInfoString</key>
	<string>Created by Qt/QMake</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleExecutable</key>
	<string>scidavis</string>
	<key>CFBundleIdentifier</key>
	<string>net.sourceforge.scidavis</string>
        <key>LSEnvironment</key>
        <dict>
           <key>PYTHONHOME</key>
           <string>/Applications/scidavis.app/Contents/Resources</string>
        </dict>
</dict>
</plist>
EOF

pkgbuild --root scidavis/scidavis.app --install-location /Applications/scidavis.app --identifier SciDAVis scidavis.pkg

