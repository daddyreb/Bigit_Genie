@echo On
echo : Compiling and Building Windows specific plugins (Release|win32)������������
echo :............Please wait .............."
"devenv.exe" GeniePlugin_VC.sln /rebuild "Release|win32" 
echo : Compiling done.
pause