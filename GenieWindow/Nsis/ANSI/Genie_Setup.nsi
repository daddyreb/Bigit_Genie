;-----------------------------------------
; NETGEAR genie Install/Unistall Program
; author:peter.zhang 
; time:2010.7.23 	update:
;-----------------------------------------

; Define your application name
!define APPNAME "NETGEAR genie"
!define VERSION "V2.1.11 Alpha"
!define APPNAMEANDVERSION "${APPNAME}"

XPstyle on
;Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\NETGEAR genie"
InstallDirRegKey HKLM "Software\${APPNAME}" "Install_Dir"

OutFile "genie_setup.exe"
BrandingText "${APPNAMEANDVERSION}"
; Use compression
SetCompressor LZMA

; Modern interface settings
!include "Sections.nsh"
!include "MUI.nsh"

;Interface Settings
!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "Graphics\bmps\genie_title.bmp" 

# MUI defines
!define MUI_ICON "Graphics\icons\genie_install.ico"         
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_LICENSEPAGE_RADIOBUTTONS
!define MUI_COMPONENTSPAGE_SMALLDESC          
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\Launch.exe"

!define MUI_UNICON "Graphics\Icons\genie_uninstall.ico"    
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

!define MUI_LANGDLL_REGISTRY_ROOT HKLM
!define MUI_LANGDLL_REGISTRY_KEY ${REGKEY}
!define MUI_LANGDLL_REGISTRY_VALUENAME InstallerLanguage

; Defines the installer /uninstall logo of Genie
!insertmacro MUI_DEFAULT MUI_WELCOMEFINISHPAGE_BITMAP "Graphics\bmps\genie_install.bmp"
!insertmacro MUI_DEFAULT MUI_UNWELCOMEFINISHPAGE_BITMAP "Graphics\bmps\genie_uninstall.bmp"

; Set languages (first is default language)
!insertmacro MUI_RESERVEFILE_LANGDLL
ReserveFile "${NSISDIR}\Plugins\AdvSplash.dll"

;--------------------------------
;Configuration

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license\license.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


# Installer languages
!define MUI_LANGDLL_ALLLANGUAGES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "Spanish"

;Component-selection page
    ;Titles
    ;English
    LangString multilan_title ${LANG_ENGLISH} "Installer Language"
	LangString multilan_info ${LANG_ENGLISH} "Please select the language of the installer"
	LangString install_info ${LANG_ENGLISH} "NETGEAR genie will be closed,are you sure?"
	LangString uninstall_info ${LANG_ENGLISH} "NETGEAR genie will be closed,are you sure?"
	LangString LANGUAGEID ${LANG_ENGLISH} "1033"
	
	;Simplified Chinese
    LangString multilan_title ${LANG_SIMPCHINESE} "安装语言"
	LangString multilan_info ${LANG_SIMPCHINESE} "请您选择安装语言"
	LangString install_info ${LANG_SIMPCHINESE} "安装程序将自动关闭NETGEAR genie,是否继续?"
	LangString uninstall_info ${LANG_SIMPCHINESE} "卸载将自动关闭NETGEAR genie,是否继续?"
	LangString LANGUAGEID ${LANG_SIMPCHINESE} "2052"
	
    ;German 
    LangString multilan_title ${LANG_GERMAN} "Installer Language"
	LangString multilan_info ${LANG_GERMAN} "Please select the language of the installer"
	LangString install_info ${LANG_GERMAN} "Please close NETGEAR genie before installation!"
	LangString uninstall_info ${LANG_GERMAN} "Please close NETGEAR genie before uninstall"
    LangString LANGUAGEID ${LANG_GERMAN} "1031"
	
   ;Portuguese
    LangString multilan_title ${LANG_PORTUGUESE} "Installer Language"
	LangString multilan_info ${LANG_PORTUGUESE} "Please select the language of the installer"
	LangString install_info ${LANG_PORTUGUESE} "NETGEAR genie will be closed,are you sure?"
	LangString uninstall_info ${LANG_PORTUGUESE} "NETGEAR genie will be closed,are you sure?"
	LangString LANGUAGEID ${LANG_PORTUGUESE} "2070"
	
	;Spanish
	LangString multilan_title ${LANG_SPANISH} "Installer Language"
	LangString multilan_info ${LANG_SPANISH} "Please select the language of the installer"
	LangString install_info ${LANG_SPANISH} "NETGEAR genie will be closed,are you sure?"
	LangString uninstall_info ${LANG_SPANISH} "NETGEAR genie will be closed,are you sure?"
	LangString LANGUAGEID ${LANG_SPANISH} "3082"
	
;-------------------------------------------------
;multilanguage selection
;-------------------------------------------------
Function .onInit
	;--------------------------------
	;check if the program is running
	;--------------------------------
	InitPluginsDir
	FindProcDLL::FindProc "Launch.exe"
    StrCmp $R0 1 +3 +9
	FindProcDLL::FindProc "Launch.exe"
    StrCmp $R0 1 +1 +7
     MessageBox MB_OKCANCEL "$(install_info)" IDOK OK IDCANCEL Cancel
     OK:
	 KillProcDLL::KillProc "Launch.exe"
	 Sleep 1000
	 Goto Continue
     Cancel:
     Quit
	;--------------------------------------------
	;welcome flash
	;---------------------------------------------
	Continue:
    Push $R1
    File /oname=$PLUGINSDIR\spltmp.bmp "Graphics\bmps\welcome_logo.bmp"
    advsplash::show 1200 1000 1000 -1 $PLUGINSDIR\spltmp
    Pop $R1
    Pop $R1
	
FunctionEnd

!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
;main section
Section $(sec_main) sec_main

  ;Set Section required
  SectionIn RO

  ; Set Section properties
  SetOverwrite on
  
  ; Set Section Files and Shortcuts
  SetOutPath "$INSTDIR"
  File /r "genie\*.*"
  
SectionEnd

SectionGroup $(sec_shortcuts) sec_shortcuts
Section  StartMenu SEC0001
   
  SetOutPath "$INSTDIR\bin"
  CreateShortCut "$SMPROGRAMS\${APPNAME}.lnk" "$INSTDIR\bin\Launch.exe" "" "$INSTDIR\bin\Launch.exe" 0						
SectionEnd

Section  Desktop SEC0002

  CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\bin\Launch.exe" "" "$INSTDIR\bin\Launch.exe" 0
  
SectionEnd

Section  Quicklaunchbar SEC0003

  CreateShortCut "$QUICKLAUNCH\${APPNAME}.lnk" "$INSTDIR\bin\Launch.exe" "" "$INSTDIR\bin\Launch.exe" 0
  
SectionEnd

SectionGroupEnd        

Section $(sec_autostart) sec_autostart

  CreateShortCut "$SMSTARTUP\${APPNAME}.lnk" "$INSTDIR\bin\Launch.exe" "-mini" "$INSTDIR\bin\Launch.exe" 0
SectionEnd

Section -FinishSection

  WriteRegStr HKLM "Software\${APPNAME}" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "Software\${APPNAME}" "Version" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
SectionEnd
;--------------------------------------------------
;Uninstall section
;--------------------------------------------------
Section "Uninstall"
  ; Remove program/uninstall regsitry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

  ;DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}.lnk"

  ; Remove desktop shortcut
  Delete "$DESKTOP\${APPNAME}.lnk"
  
  ; Remove Quicklaunch shortcut
  Delete "$QUICKLAUNCH\${APPNAME}.lnk"
  
  ; Remove Autstart 
  Delete "$SMSTARTUP\${APPNAME}.lnk"
  
  ; Remove directories used
  ;RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir /r "$INSTDIR"

SectionEnd

Function  un.onInit 
    FindProcDLL::FindProc "Launch.exe" 
    Pop $R0 
    IntCmp $R0 1 0 no_run 
    MessageBox MB_OKCANCEL "$(uninstall_info)" IDOK OK IDCANCEL Cancel
	OK:
	KillProcDLL::KillProc "Launch.exe"
	Sleep 1000
	Goto no_run
	Cancel:
    Quit 
    no_run: 
FunctionEnd

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall"
LangString ^UninstallLink ${LANG_GERMAN} "Unistall"
LangString ^UninstallLink ${LANG_PORTUGUESE} "Unistall"
LangString ^UninstallLink ${LANG_FRENCH} "Uninstall"
LangString ^UninstallLink ${LANG_SPANISH} "Uninstall"
LangString ^UninstallLink ${LANG_SIMPCHINESE} "卸载"
;LangString ^UninstallLink ${LANG_TRADCHINESE} "卸载"

; eof