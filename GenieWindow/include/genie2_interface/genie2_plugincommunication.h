#ifndef GENIE2_COMMUNICATIONS__H__
#define GENIE2_COMMUNICATIONS__H__
#include "interface2_cmds.h"
enum _E_PLUGIN_COMMUNICATION_CMDS
{
EPCC_START=I2CMD_OTHER1
,EPCC_ROUTERCONFIGUI //plugin routerconfiguration ask subplugin,then subplugin return a ui pointer
,EPCC_WIRELESSEXPORT_DATA //plugin routerconfiguration tell subplugin wireless export the data to export
,EPCC_TURNTO_EXPORT //plugin routerconfiguration tell subplubin wirelessexport,when click button "export"
,EPCC_TURN_TO_CHANNEL//plugin wireless tell plugin routerconfiguration to change wireless channel
//,EPCC_TURN_TO_READYSHARE  //plugin ui tell plugin routerconfiguration to turn to page readyshare
,EPCC_TURN_TO_GACC //plugin wireless tell plugin routerconfigurasion to turn to page guest access
,EPCC_TURN_TO_WIRELESS_PWD //plugin routerconfigurastion receive this to turn to page wireless info
,EPCC_CONNECT_TO_SSID //plugin routerconfiguration tell plugin wireless to connect a ssid
,EPCC_SSID_KEY //when plugin routerconfiguration change the wireless setting,then tell the plugin wireless to change profiles
,EPCC_SOFTWARE_UPDATE //subplugin update receive this command to start software update
,EPCC_FIRMWARE_UPDATE //plugin routerconfigurasion to start firmwareupdate checker
,EPCC_WIRELESSEXPORT_RETURN //when plugin wireless export process end,tell plugin wireless to turn to origin page
,EPCC_SMARTNETWORK_DATA //plugin routerconfig tell plugin lpc,the status of smartnetwork
,EPCC_END
};

#endif
