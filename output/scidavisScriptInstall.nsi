;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  File                 : scidavisScriptInstall.nsi
;  Project              : SciDAVis
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Copyright            : (C) 2008 by Mauricio Troviano
;  Email (use @ for *)  : m.troviano*gmail.com
;  Description          : Script for the nullsoft scriptable installation system
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                         ;
;  This program is free software; you can redistribute it and/or modify   ;
;  it under the terms of the GNU General Public License as published by   ;
;  the Free Software Foundation; either version 2 of the License, or      ;
;  (at your option) any later version.                                    ;
;                                                                         ;
;  This program is distributed in the hope that it will be useful,        ;
;  but WITHOUT ANY WARRANTY; without even the implied warranty of         ;
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          ;
;  GNU General Public License for more details.                           ;
;                                                                         ;
;   You should have received a copy of the GNU General Public License     ;
;   along with this program; if not, write to the Free Software           ;
;   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    ;
;   Boston, MA  02110-1301  USA                                           ;
;                                                                         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "SciDAVis"
!define PRODUCT_VERSION "1.D5"
!define PRODUCT_WEB_SITE "http://scidavis.sourceforge.net/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\scidavis.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define PYTHON_INSTALLER "python-2.7.6.msi"
!define PYTHON_URL "http://www.python.org/ftp/python/2.7.6/${PYTHON_INSTALLER}"

SetCompressor /SOLID lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "scidavis.ico"
;!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNICON "scidavis.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "gpl.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\scidavis.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "scidavis-${PRODUCT_VERSION}-win32-setup.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "SciDAVis" SEC01
  SectionIn RO
  SetOutPath "$INSTDIR"
  CreateDirectory $INSTDIR\plugins
  CreateDirectory $INSTDIR\translations
  CreateDirectory $INSTDIR\PyQt4
  SetOverwrite try

  File "CHANGES.txt"
  File "gpl.txt"
  File "mingwm10.dll"
  File "libgcc_s_dw2-1.dll"
  File "Qt3Support4.dll"
  File "QtAssistantClient4.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtNetwork4.dll"
  File "QtOpenGL4.dll"
  File "QtSql4.dll"
  File "QtSvg4.dll"
  File "QtXml4.dll"
  File "qwtplot3d.dll"
  File "README.txt"
  File "scidavis.ico"
  File "scidavis.exe"
  File "scidavisrc.py"
  File "scidavisrc.pyc"
  File "scidavisUtil.py"
  ;File "scidavisUtil.pyc"
  File "assistant.exe"
  File "sip.pyd"
  File "python27.dll"
  File "zlib1.dll"
  SetOutPath "$INSTDIR\plugins"
  File "plugins\fitRational0.dll"
  File "plugins\fitRational1.dll"
  File "plugins\exp_saturation.dll"
  File "plugins\explin.dll"
  SetOutPath "$INSTDIR\translations"
  File "translations\scidavis_de.qm"
  File "translations\scidavis_es.qm"
  File "translations\scidavis_fr.qm"
  File "translations\scidavis_ja.qm"
  File "translations\scidavis_ru.qm"
  File "translations\scidavis_sv.qm"
  File "translations\scidavis_pt-br.qm"
  File "translations\scidavis_cs.qm"
  File "translations\scidavis_cs-alt.qm"
  SetOutPath "$INSTDIR\PyQt4"
  File "PyQt4\__init__.py"
  File "PyQt4\__init__.pyc"
  File "PyQt4\Qt.pyd"
  File "PyQt4\QtAssistant.pyd"
  File "PyQt4\QtCore.pyd"
  File "PyQt4\QtDesigner.pyd"
  File "PyQt4\QtGui.pyd"
  File "PyQt4\QtHelp.pyd"
  File "PyQt4\QtNetwork.pyd"
  File "PyQt4\QtOpenGL.pyd"
  File "PyQt4\QtScript.pyd"
  File "PyQt4\QtSql.pyd"
  File "PyQt4\QtSvg.pyd"
  File "PyQt4\QtTest.pyd"
  File "PyQt4\QtWebKit.pyd"
  File "PyQt4\QtXml.pyd"
  File "PyQt4\QtXmlPatterns.pyd"
  
  
; Shortcuts
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\${PRODUCT_NAME}.lnk" "$INSTDIR\scidavis.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section /o "Desktop shortcut" SEC02
  SectionIn 1
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\scidavis.exe"
SectionEnd

LangString DESC_Section1 ${LANG_ENGLISH} "The application."
LangString DESC_Section2 ${LANG_ENGLISH} "Create desktop shortcut."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} $(DESC_Section2)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\scidavis.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\scidavis.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\scidavisUtil.pyc"
  Delete "$INSTDIR\scidavisUtil.py"
  Delete "$INSTDIR\scidavisrc.pyc"
  Delete "$INSTDIR\scidavisrc.py"
  Delete "$INSTDIR\scidavis.exe"
  Delete "$INSTDIR\scidavis.ico"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\qwtplot3d.dll"
  Delete "$INSTDIR\QtXml4.dll"
  Delete "$INSTDIR\QtSvg4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\QtOpenGL4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtAssistantClient4.dll"
  Delete "$INSTDIR\Qt3Support4.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\gpl.txt"
  Delete "$INSTDIR\CHANGES.txt"
  Delete "$INSTDIR\assistant.exe"
  Delete "$INSTDIR\sip.pyd"
  Delete "$INSTDIR\python27.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\plugins\fitRational1.dll"
  Delete "$INSTDIR\plugins\fitRational0.dll"
  Delete "$INSTDIR\plugins\exp_saturation.dll"
  Delete "$INSTDIR\plugins\explin.dll"
  Delete "$INSTDIR\translations\scidavis_de.qm"
  Delete "$INSTDIR\translations\scidavis_es.qm"
  Delete "$INSTDIR\translations\scidavis_fr.qm"
  Delete "$INSTDIR\translations\scidavis_ja.qm"
  Delete "$INSTDIR\translations\scidavis_ru.qm"
  Delete "$INSTDIR\translations\scidavis_sv.qm"
  Delete "$INSTDIR\translations\scidavis_pt-br.qm"
  Delete "$INSTDIR\translations\scidavis_cs.qm"
  Delete "$INSTDIR\translations\scidavis_cs-alt.qm"
  Delete "$INSTDIR\PyQt4\__init__.py"
  Delete "$INSTDIR\PyQt4\__init__.pyc"
  Delete "$INSTDIR\PyQt4\Qt.pyd"
  Delete "$INSTDIR\PyQt4\QtAssistant.pyd"
  Delete "$INSTDIR\PyQt4\QtCore.pyd"
  Delete "$INSTDIR\PyQt4\QtDesigner.pyd"
  Delete "$INSTDIR\PyQt4\QtGui.pyd"
  Delete "$INSTDIR\PyQt4\QtHelp.pyd"
  Delete "$INSTDIR\PyQt4\QtNetwork.pyd"
  Delete "$INSTDIR\PyQt4\QtOpenGL.pyd"
  Delete "$INSTDIR\PyQt4\QtScript.pyd"
  Delete "$INSTDIR\PyQt4\QtSql.pyd"
  Delete "$INSTDIR\PyQt4\QtSvg.pyd"
  Delete "$INSTDIR\PyQt4\QtTest.pyd"
  Delete "$INSTDIR\PyQt4\QtWebKit.pyd"
  Delete "$INSTDIR\PyQt4\QtXml.pyd"
  Delete "$INSTDIR\PyQt4\QtXmlPatterns.pyd"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\${PRODUCT_NAME}.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR\plugins"
  RMDir "$INSTDIR\translations"
  RMDir "$INSTDIR\PyQt4"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

; Ask whether to install Python
Section -Requirements
  SetOutPath $INSTDIR
  ReadRegStr $3 HKLM "SOFTWARE\Python\PythonCore\2.7\InstallPath" ""
  StrCmp $3 "" 0 pythonInstFound
  ReadRegStr $3 HKCU "SOFTWARE\Python\PythonCore\2.7\InstallPath" ""
  StrCmp $3 "" 0 pythonInstFound
  StrCpy $4 "No Python 2.7 found on your system. Install Python 2.7 from the internet?"
  MessageBox MB_YESNO $4 /SD IDYES IDNO endInstPython
        StrCpy $2 "$TEMP\${PYTHON_INSTALLER}"
        nsisdl::download /TIMEOUT=30000 ${PYTHON_URL} $2
        Pop $R0 ;Get the return value
                StrCmp $R0 "success" +3
                MessageBox MB_OK "Download failed: $R0"
                Quit
	ExecWait '"msiexec" /i "$2"'
        Delete $2
  pythonInstFound:
  endInstPython:
SectionEnd
