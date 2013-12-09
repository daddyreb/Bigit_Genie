if exist ..\lib (xcopy ..\lib ..\bin /s /e /Y) else pause
cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Rebuild "Release|Win32"
devenv GeniePlugin_VC.sln /Rebuild "Release|x64"
cd ..\SvtNetworkTool
qmake 
mingw32-make clean
mingw32-make release
cd ..\ComputerDetails\ComputerDetails
qmake
mingw32-make clean
mingw32-make release
cd ..

cd  ..\DiagnosePlugin
qmake 
mingw32-make clean
mingw32-make release

cd ..\DragonNetTool
qmake 
mingw32-make clean
mingw32-make release

cd ..
cd genie2_qrcode
qmake 
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_Airprint
qmake
mingw32-make clean
mingw32-make release
cd ..\GeniePlugin_Internet
qmake 
mingw32-make clean
mingw32-make release

cd ..\topology
qmake
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_MapV2\howl
qmake
mingw32-make clean
mingw32-make release

cd ..\dlna
qmake
mingw32-make clean
mingw32-make release

cd ..
qmake
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_LPC
qmake 
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_NetworkProblem
qmake  
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_Resource
qmake
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_RouterConfiguration
qmake 
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_Statistics
qmake 
mingw32-make clean
mingw32-make release
cd ..\GeniePlugin_Ui
qmake
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_Update2
qmake 
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_Wireless
qmake
mingw32-make clean
mingw32-make release

cd ..\WSetupApiPlugin
qmake 
mingw32-make clean
mingw32-make release

cd ..\readyShareFileChecker
qmake
mingw32-make clean
mingw32-make release

cd ..\InternetDaemon
qmake
mingw32-make clean
mingw32-make release

cd ..\GeniePlugin_WirelessExport
qmake
mingw32-make clean
mingw32-make release

cd ..\genie_module
qmake  
mingw32-make clean
mingw32-make release

cd ..\..\genie2_tray
qmake
mingw32-make clean
mingw32-make release

cd ..\launch
qmake
mingw32-make clean
mingw32-make release
cd ..\setup
package_mingw_release.bat