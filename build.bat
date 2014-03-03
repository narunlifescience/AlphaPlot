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

rem ========================= BUILD AND INSTALL LIBORIGIN ===========================
cd 3rdparty\liborigin
cmake -G "MinGW Makefiles"
mingw32-make
mkdir build
copy liborigin.a .\build
cd ..\..
rem =================== BUILD AND INSTALL SCIDAVIS ============================
qmake scidavis.pro
mingw32-make release

mingw32-make INSTALL_ROOT=./output install

rem ============ COPY SOME MISSING FILES TO OUTPUT DIRECTORY ==================
copy .\3rdparty\qwtplot3d\lib\qwtplot3d.dll .\output
copy .\3rdparty\zlib\bin\zlib1.dll .\output


copy .\scidavis\scidavis.exe .\output
copy .\scidavis\icons\scidavis.ico	   .\output
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
copy /y %QTDIR%\bin\libgcc_s_dw2-1.dll	   output
copy .\gpl.txt .\output	
copy .\README .\output\README.txt
copy .\CHANGES .\output\CHANGES.txt
copy .\scidavis\scidavisrc.py .\output
copy .\scidavis\scidavisUtil.py .\output
copy C:\Python27\Lib\site-packages\sip.pyd .\output
copy C:\Python27\DLLs\python27.dll .\output