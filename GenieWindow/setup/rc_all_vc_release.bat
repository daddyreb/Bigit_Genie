if exist ..\lib (xcopy ..\lib ..\bin /s /e /Y) else pause
cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Rebuild "Release|Win32"
devenv GeniePlugin_VC.sln /Rebuild "Release|x64"
cd ..\SvtNetworkTool
qmake 
nmake clean
nmake release
cd ..\ComputerDetails\ComputerDetails
qmake
nmake clean
nmake release
cd ..

cd ..\DiagnosePlugin
qmake 
nmake clean
nmake release
cd ..\DragonNetTool
qmake 
nmake clean
nmake release
cd ..
cd genie2_qrcode
qmake 
nmake clean
nmake release
cd ..\GeniePlugin_Airprint
qmake
nmake clean
nmake release
cd ..\GeniePlugin_Internet
qmake 
nmake clean
nmake release
cd ..\topology
qmake
nmake clean
nmake release
cd ..\GeniePlugin_MapV2\howl
qmake
nmake clean
nmake release
cd ..\dlna
qmake
nmake clean
nmake release
cd ..
qmake
nmake clean
nmake release
cd ..\GeniePlugin_LPC
qmake 
nmake clean
nmake release
cd ..\GeniePlugin_NetworkProblem
qmake  
nmake clean
nmake release
cd ..\GeniePlugin_Resource
qmake
nmake clean
nmake release
cd ..\GeniePlugin_RouterConfiguration
qmake 
nmake clean
nmake release
cd ..\GeniePlugin_Statistics
qmake 
nmake clean
nmake release
cd ..\GeniePlugin_Ui
qmake
nmake clean
nmake release
cd ..\GeniePlugin_Update2
qmake 
nmake clean
nmake release
cd ..\GeniePlugin_Wireless
qmake
nmake clean
nmake release
cd ..\WSetupApiPlugin
qmake 
nmake clean
nmake release
cd ..\readyShareFileChecker
qmake
nmake clean
nmake release
cd ..\InternetDaemon
qmake
nmake clean
nmake release
cd ..\GeniePlugin_WirelessExport
qmake
nmake clean
nmake release
cd ..\genie_module
qmake  
nmake clean
nmake release

cd ..\..\genie2_tray
qmake
nmake clean
nmake release
cd ..\launch
qmake
nmake clean
nmake release
cd ..\setup
package_vc_release.bat
