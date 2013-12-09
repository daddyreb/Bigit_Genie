@echo off
cd ..\plugins\GeniePlugin_VC
set path =%cd%
echo 正在编译VC项目..............
"devenv.exe" GeniePlugin_VC.sln /rebuild "Release|win32" 
echo %path%
cd..\..
set path =%cd%
echo %path%
echo Clean Genie............
mingw32-make.exe clean
echo 正在编译Qt项目...........
qmake.exe Genie.pro -r -spec win32-g++ CONFIG+=release
mingw32-make.exe release 
cd bin
del *.a
rd Debug /s /q
pause