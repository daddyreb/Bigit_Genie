if exist ..\lib (xcopy ..\lib ..\bin /s /e /Y) else pause
cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Rebuild "Debug|Win32"
devenv GeniePlugin_VC.sln /Rebuild "Debug|x64"
cd ..\SvtNetworkTool
qmake 
mingw32-make clean
mingw32-make debug

cd ..\ComputerDetails\ComputerDetails
qmake
mingw32-make clean
mingw32-make debug
cd ..

cd ..\DiagnosePlugin
qmake 
mingw32-make clean
mingw32-make debug
cd ..\DragonNetTool
qmake 
mingw32-make clean
mingw32-make debug
cd ..
cd genie2_qrcode
qmake 
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Airprint
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Internet
qmake 
mingw32-make clean
mingw32-make debug
cd ..\topology
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_MapV2\howl
qmake
mingw32-make clean
mingw32-make debug
cd ..\dlna
qmake
mingw32-make clean
mingw32-make debug
cd ..
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_LPC
qmake 
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_NetworkProblem
qmake  
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Resource
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_RouterConfiguration
qmake 
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Statistics
qmake 
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Ui
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Update2
qmake 
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_Wireless
qmake
mingw32-make clean
mingw32-make debug
cd ..\WSetupApiPlugin
qmake 
mingw32-make clean
mingw32-make debug
cd ..\readyShareFileChecker
qmake
mingw32-make clean
mingw32-make debug
cd ..\InternetDaemon
qmake
mingw32-make clean
mingw32-make debug
cd ..\GeniePlugin_WirelessExport
qmake
mingw32-make clean
mingw32-make debug
cd ..\genie_module
qmake  
mingw32-make clean
mingw32-make debug

cd ..\..\genie2_tray
qmake
mingw32-make clean
mingw32-make debug
cd ..\launch
qmake
mingw32-make clean
mingw32-make debug
cd ..\setup
package_mingw_debug.bat