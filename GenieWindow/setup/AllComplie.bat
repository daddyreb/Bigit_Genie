@echo off
cd ..\plugins\GeniePlugin_VC
set path =%cd%
echo ���ڱ���VC��Ŀ..............
"devenv.exe" GeniePlugin_VC.sln /rebuild "Release|win32" 
echo %path%
cd..\..
set path =%cd%
echo %path%
echo Clean Genie............
mingw32-make.exe clean
echo ���ڱ���Qt��Ŀ...........
qmake.exe Genie.pro -r -spec win32-g++ CONFIG+=release
mingw32-make.exe release 
cd bin
del *.a
rd Debug /s /q
pause