rem ============================ BUILD MUPARSER ===============================
cd 3rdparty\muparser\build
mingw32-make -fmakefile.mingw
cd ..\..\..

rem ========================= BUILD AND INSTALL SIP ===========================
cd 3rdparty\sip
call configure.py -p win32-g++
mingw32-make
mingw32-make install
cd ..\..

rem ========================= BUILD AND INSTALL PYQT ==========================
cd 3rdparty\PyQt-win-gpl
call configure.py --confirm-license
mingw32-make
mingw32-make install
cd ..\..

rem ============= PATCH CONFIGURATION FILES OF QWT AND QWTPLOT3D ==============
cd 3rdparty
call patch-configs.py
cd ..

rem ============================= BUILD QWT ===================================
cd 3rdparty\qwt
qmake
mingw32-make
cd ..\..

rem ========================= BUILD QWTPLOT3D =================================
cd 3rdparty\qwtplot3d
qmake
mingw32-make
cd ..\..

rem =================== BUILD AND INSTALL SCIDAVIS ============================
qmake scidavis.pro
mingw32-make
mingw32-make INSTALL_ROOT=../output install

rem ============ COPY SOME MISSING FILES TO OUTPUT DIRECTORY ==================
copy /y %QTDIR%\bin\mingwm10.dll           output
copy /y %QTDIR%\bin\Qt3Support4.dll        output
copy /y %QTDIR%\bin\QtAssistantClient4.dll output
copy /y %QTDIR%\bin\QtCore4.dll            output
copy /y %QTDIR%\bin\QtGui4.dll             output
copy /y %QTDIR%\bin\QtNetwork4.dll         output
copy /y %QTDIR%\bin\QtOpenGL4.dll          output
copy /y %QTDIR%\bin\QtSql4.dll             output
copy /y %QTDIR%\bin\QtSvg4.dll             output
copy /y %QTDIR%\bin\QtXml4.dll             output
copy /y %QTDIR%\bin\assistant.exe          output
