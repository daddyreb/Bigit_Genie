cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Build "Release|Win32"
devenv GeniePlugin_VC.sln /Build "Release|x64"
cd ..\SvtNetworkTool
qmake 
nmake release
cd ..\ComputerDetails\ComputerDetails
qmake
nmake release
cd ..
cd ..\DiagnosePlugin
qmake 
nmake release
cd ..\DragonNetTool
qmake 
nmake release
cd ..
cd genie2_qrcode
qmake 
nmake release
cd ..\GeniePlugin_Airprint
qmake
nmake release
cd ..\GeniePlugin_Internet
qmake 
nmake release
cd ..\topology
qmake
nmake release
cd ..\GeniePlugin_MapV2\howl
qmake
nmake release
cd ..\dlna
qmake
nmake release
cd ..
qmake
nmake release
cd ..\GeniePlugin_LPC
qmake 
nmake release
cd ..\GeniePlugin_NetworkProblem
qmake  
nmake release
cd ..\GeniePlugin_Resource
qmake
nmake release
cd ..\GeniePlugin_RouterConfiguration
qmake 
nmake release
cd ..\GeniePlugin_Statistics
qmake 
nmake release
cd ..\GeniePlugin_Ui
qmake
nmake release
cd ..\GeniePlugin_Update2
qmake 
nmake release
cd ..\GeniePlugin_Wireless
qmake
nmake release
cd ..\WSetupApiPlugin
qmake 
nmake release
cd ..\readyShareFileChecker
qmake
nmake release
cd ..\InternetDaemon
qmake
nmake release
cd ..\GeniePlugin_WirelessExport
qmake
nmake release
cd ..\genie_module
qmake  
nmake release

cd ..\..\genie2_tray
qmake
nmake release
cd ..\launch
qmake
nmake release
cd ..\setup
package_vc_release.bat