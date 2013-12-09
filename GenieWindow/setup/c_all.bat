cd ..\plugins\GeniePlugin_VC
devenv GeniePlugin_VC.sln /Rebuild "Release|Win32"
devenv GeniePlugin_VC.sln /Rebuild "Release|x64"
cd ..\SvtNetworkTool
qmake -t vclib -o SvtNetworkTool.vcxproj  SvtNetworkTool.pro
devenv.exe SvtNetworkTool.vcxproj /rebuild "Debug|win32" 
cd ..\ComputerDetails\ComputerDetails
qmake
nmake debug
cd ..
cd  ..\DiagnosePlugin
qmake 
nmake debug
cd ..\DragonNetTool
qmake 
nmake debug
cd ..\genie2_qrcode
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
qmake  -t vclib -o GeniePlugin_NetWorkProblem.vcxproj GeniePlugin_NetWorkProblem.pro
devenv.exe GeniePlugin_NetWorkProblem.vcxproj /rebuild "Debug|win32" 
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
