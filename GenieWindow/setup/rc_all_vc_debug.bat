if exist ..\lib (xcopy ..\lib ..\bin /s /e /Y) else pause
cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Rebuild "Debug|Win32"
devenv GeniePlugin_VC.sln /Rebuild "Debug|x64"
cd ..\SvtNetworkTool
qmake 
nmake clean
nmake debug
cd ..\ComputerDetails\ComputerDetails
qmake
nmake clean
nmake debug
cd ..
cd ..\DiagnosePlugin
qmake 
nmake clean
nmake debug
cd ..\DragonNetTool
qmake 
nmake clean
nmake debug
cd ..
cd genie2_qrcode
qmake 
nmake clean
nmake debug
cd ..\GeniePlugin_Airprint
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_Internet
qmake 
nmake clean
nmake debug
cd ..\topology
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_MapV2\howl
qmake
nmake clean
nmake debug
cd ..\dlna
qmake
nmake clean
nmake debug
cd ..
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_LPC
qmake 
nmake clean
nmake debug
cd ..\GeniePlugin_NetworkProblem
qmake  
nmake clean
nmake debug
cd ..\GeniePlugin_Resource
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_RouterConfiguration
qmake 
nmake clean
nmake debug
cd ..\GeniePlugin_Statistics
qmake 
nmake clean
nmake debug
cd ..\GeniePlugin_Ui
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_Update2
qmake 
nmake clean
nmake debug
cd ..\GeniePlugin_Wireless
qmake
nmake clean
nmake debug
cd ..\WSetupApiPlugin
qmake 
nmake clean
nmake debug
cd ..\readyShareFileChecker
qmake
nmake clean
nmake debug
cd ..\InternetDaemon
qmake
nmake clean
nmake debug
cd ..\GeniePlugin_WirelessExport
qmake
nmake clean
nmake debug
cd ..\genie_module
qmake  
nmake clean
nmake debug

cd ..\..\genie2_tray
qmake
nmake clean
nmake debug
cd ..\launch
qmake
nmake clean
nmake debug
cd ..\setup
package_vc_debug.bat