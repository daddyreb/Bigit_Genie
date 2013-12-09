del /S /Q ..\Nsis\genie\bin\
xcopy ..\bin\*.dll ..\Nsis\genie\bin\ /Y
xcopy ..\bin\*.exe ..\Nsis\genie\bin\ /Y
mkdir ..\Nsis\genie\bin\drivers
if exist ..\Nsis\drivers (xcopy ..\Nsis\drivers ..\Nsis\genie\bin\drivers\ /s /e /Y) else pause
if exist e:\Qt5Runtime\mingwDebug (xcopy e:\Qt5Runtime\mingwDebug ..\Nsis\genie\bin\ /s /e /Y) else pause

if not exist ..\Nsis\genie\bin\genie2_tray.exe pause
if not exist ..\Nsis\genie\bin\InternetDaemon.exe pause
if not exist ..\Nsis\genie\bin\NETGEARGenieDaemon.exe pause
if not exist ..\Nsis\genie\bin\NETGEARGenieDaemon64.exe pause
if not exist ..\Nsis\genie\bin\readyShareFileChecker.exe pause

if not exist ..\Nsis\genie\bin\airprintdll.dll pause
if not exist ..\Nsis\genie\bin\ComputerDetails.dll pause
if not exist ..\Nsis\genie\bin\DiagnoseDll.dll pause
if not exist ..\Nsis\genie\bin\DiagnoseDll.dll pause
if not exist ..\Nsis\genie\bin\DragonNetTool.dll pause
if not exist ..\Nsis\genie\bin\Genie.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Airprint.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Internet.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Map.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_NetworkProblem.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_ParentalControl.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Resource.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_RouterConfiguration.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Statistics.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Ui.dll pause
if not exist ..\Nsis\genie\bin\GeniePlugin_Wireless.dll pause
if not exist ..\Nsis\genie\bin\InnerPlugin_Update.dll pause
if not exist ..\Nsis\genie\bin\InnerPlugin_WirelessExport.dll pause
if not exist ..\Nsis\genie\bin\NetcardApi.dll pause
if not exist ..\Nsis\genie\bin\SvtNetworkTool.dll pause
if not exist ..\Nsis\genie\bin\SVTUtils.dll pause
if not exist ..\Nsis\genie\bin\WiFiApi.dll pause
if not exist ..\Nsis\genie\bin\WSetupApiPlugin.dll pause
if not exist ..\Nsis\genie\bin\WSetupDll.dll pause

makensis ../Nsis/Genie_Setup.nsi