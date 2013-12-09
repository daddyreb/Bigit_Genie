cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Build "Debug|Win32"
devenv GeniePlugin_VC.sln /Build "Debug|x64"
cd ..\SvtNetworkTool
qmake 
nmake debug
cd ..\ComputerDetails\ComputerDetails
qmake
nmake debug
cd  ..\..\DiagnosePlugin
qmake 
nmake debug
cd ..\DragonNetTool
qmake 
nmake debug
cd ..
cd genie2_qrcode
qmake 
nmake debug
cd ..\GeniePlugin_Airprint
qmake
nmake debug
cd ..\GeniePlugin_Internet
qmake 
nmake debug
cd ..\topology
qmake
nmake debug
cd ..\GeniePlugin_MapV2\howl
qmake
nmake debug
cd ..\dlna
qmake
nmake debug
cd ..
qmake
nmake debug
cd ..\GeniePlugin_LPC
qmake 
nmake debug
cd ..\GeniePlugin_NetworkProblem
qmake  
nmake debug
cd ..\GeniePlugin_Resource
qmake
nmake debug
cd ..\GeniePlugin_RouterConfiguration
qmake 
nmake debug
cd ..\GeniePlugin_Statistics
qmake 
nmake debug
cd ..\GeniePlugin_Ui
qmake
nmake debug
cd ..\GeniePlugin_Update2
qmake 
nmake debug
cd ..\GeniePlugin_Wireless
qmake
nmake debug
cd ..\WSetupApiPlugin
qmake 
nmake debug
cd ..\readyShareFileChecker
qmake
nmake debug
cd ..\InternetDaemon
qmake
nmake debug
cd ..\GeniePlugin_WirelessExport
qmake
nmake debug
cd ..\genie_module
qmake
nmake debug

cd ..\..\genie2_tray
qmake
nmake debug
cd ..\launch
qmake
nmake debug
cd ..\setup
package_vc_debug.bat