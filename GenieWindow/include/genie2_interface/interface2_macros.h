#ifndef INTERFACE2_MACROS_H
#define INTERFACE2_MACROS_H

#ifndef GENIE2_SAFEDELETE
#define GENIE2_SAFEDELETE(_p) do \
        {\
         if(_p){delete _p;_p=0;}\
        }while(0)
#endif

enum E_LANGUAGE_TYPE{
    ELT_ENGLISH=0,ELT_GERMANY,ELT_SWEDISH,ELT_FRENCH,ELT_DUTCH,ELT_ITALIAN,ELT_SPANISH,
    ELT_DANISH,ELT_FINNISH,ELT_NORWEGIAN,ELT_GREEK,ELT_PORTUGUESE,ELT_RUSSIAN,ELT_POLISH,
    ELT_HUNGARIAN,ELT_CZECH,ELT_SLOVENIAN,ELT_SLOVAK,ELT_ROMANIAN,ELT_CHINESE,ELT_KOREAN,
    ELT_JAPANESE,ELT_ARABIC,ELT_TURKISH,
    ELT_NUM,
    ELT_ERROR
};

#ifdef Q_OS_WIN32
#define PLUGIN_FILE_UPDATE "InnerPlugin_Update.dll"
#define PLUGIN_FILE_WEXPORT "InnerPlugin_WirelessExport.dll"
#else
#define PLUGIN_FILE_UPDATE "libInnerPlugin_Update.dylib"
#define PLUGIN_FILE_WEXPORT "libInnerPlugin_WirelessExport.dylib"
#endif

#define FRAME_UUID "{7b399884-b36d-4de8-ac04-19e7452023d8}"
#define PLUGIN_UUID_INFO "{f9b01961-e3b2-474c-8521-42371d2cab61}"
#define PLUGIN_UUID_STATISTIC "{6b864d58-7d53-489d-a558-1684e2fb2f60}"
#define PLUGIN_UUID_NETTRAFFIC "{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"
#define PLUGIN_UUID_NETCONFIG "{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}"
#define PLUGIN_UUID_UPDATE "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"
#define PLUGIN_UUID_INTERNET "{6c7b332f-a107-40ab-84f3-2163ac13ad0a}"
#define PLUGIN_UUID_WIRELESS "{e722b972-b05e-4d71-8cd9-676ebd1535b9}"
#define PLUGIN_UUID_MAP "{5325cdd0-d419-4e98-88d8-66d865932824}"
#define PLUGIN_UUID_UI "{6653bbdd-844a-465b-bcdb-94111206b1b2}"
#define PLUGIN_UUID_RESOURCE "{3496f5e0-89ec-43df-849b-14ae7564befa}"
#define PLUGIN_UUID_LPC "{CED77229-C59A-4669-9C26-0BB4860D96A7}"
#define PLUGIN_UUID_TRAFFICMETER "{35b9be0c-b073-4cf8-bc12-d11923dc3d49}"
#define PLUGIN_UUID_FEEDBACK "{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}"
#define PLUGIN_UUID_WIRELESSEXPORT "{f8018c9f-5431-497e-ac5d-511dd0ae7402}"
#define PLUGIN_UUID_AIRPRINT "{142d68b8-aadf-4500-9d32-7c27947bd6bb}"
#define PLUGIN_UUID_SMARTWIFI "{98DB626F-5F14-387F-0931-81B7581111A3}"



#include "inner_macros.h"


//send command to main frame,refer to _HANDLER_CMDS at interface2_cmds.h
#define GENIE2_HANDLER_CMD(_cmd,_para) __IFACE2_HANDLER_CMD(_cmd,_para)

//send message to other plugin,refer to _IFACE2_CMDS at interface2_cmds.h
#define GENIE2_TELL_OTHERPLUGIN(_uuid,_msg,_paras) __IFACE2_TELL_OTHER_PLUGIN(_uuid,_msg,_paras)

//use this macro when received I2CMD_TIMETO_CREATEUI, not somewhere else
#define GENIE2_REGISTER_UI(_p,_discription) __IFACE2_REGISTER_UI(_p,_discription)

#define GENIE2_REGISTER_DBG_UI(_p) __IFACE2_REGISTER_DBG_UI__(_p)
//send command to ui plugin,refer to _UI_SUB_CMDS at interface2_cmds.h
#define GENIE2_UI_COMMAND(_cmd,_para) __IFACE2_UI_COMMAND_MULTI_BBT(_cmd,0,_para)
#define GENIE2_UI_COMMAND2(_cmd,_para,_idx) __IFACE2_UI_COMMAND_MULTI_BBT(_cmd,_idx,_para)
#define GENIE2_SET_FRAMELESSSTYLE(_w) __IFACE2_SETFRAMELESS_STYLE_(_w)

//multi language support
#define GENIE2_TRANSLATE_LABEL(_lab,_lan) __IFACE2_TRANSLATE_(_lab,HCMD_TRANSLATE_LABEL,_lan,"")
#define GENIE2_TRANSLATE_LABEL2(_lab,_lan,_mod) __IFACE2_TRANSLATE_(_lab,HCMD_TRANSLATE_LABEL,_lan,_mod)
#define GENIE2_TRANSLATE_BUTTON(_but,_lan) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_BUTTON,_lan,"")
#define GENIE2_TRANSLATE_BUTTON2(_but,_lan,_mod) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_BUTTON,_lan,_mod)
#define GENIE2_TRANSLATE_CHECKBOX(_but,_lan) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_CHECKBOX,_lan,"")
#define GENIE2_TRANSLATE_CHECKBOX2(_but,_lan,_mod) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_CHECKBOX,_lan,_mod)
#define GENIE2_TRANSLATE_RADIOBUT(_but,_lan) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_RADIOBUT,_lan,"")
#define GENIE2_TRANSLATE_RADIOBUT2(_but,_lan,_mod) __IFACE2_TRANSLATE_(_but,HCMD_TRANSLATE_RADIOBUT,_lan,_mod)




#define GENIE2_TRANSLATE_CHILD_LABEL(_p,_objname,_lan) __IFACE2_TRANSLATE_CHILD_LAB_(_p,_objname,_lan,"")
#define GENIE2_TRANSLATE_CHILD_LABEL2(_p,_objname,_lan,_mod) __IFACE2_TRANSLATE_CHILD_LAB_(_p,_objname,_lan,_mod)

#define GENIE2_TRANSLATE_CHILD_BUTTON(_p,_objname,_lan) __IFACE2_TRANSLATE_CHILD_BUT_(_p,_objname,_lan,"")
#define GENIE2_TRANSLATE_CHILD_BUTTON2(_p,_objname,_lan,_mod) __IFACE2_TRANSLATE_CHILD_BUT_(_p,_objname,_lan,_mod)

#define GENIE2_TRANSLATE_CHILD_CHECKBOX(_p,_objname,_lan) __IFACE2_TRANSLATE_CHILD_CHECKBOX_(_p,_objname,_lan,"")
#define GENIE2_TRANSLATE_CHILD_CHECKBOX2(_p,_objname,_lan,_mod) __IFACE2_TRANSLATE_CHILD_CHECKBOX_(_p,_objname,_lan,_mod)

#define GENIE2_TRANSLATE_CHILD_RADIOBUT(_p,_objname,_lan) __IFACE2_TRANSLATE_CHILD_RADIOBUT_(_p,_objname,_lan,"")
#define GENIE2_TRANSLATE_CHILD_RADIOBUT2(_p,_objname,_lan,_mod) __IFACE2_TRANSLATE_CHILD_RADIOBUT_(_p,_objname,_lan,_mod)



#define GENIE2_GET_TEXT(_lan) _IFACE2_GET_TEXT_USINGUUID(_lan,QGenieIface2ShareObject::sUuidIdx)
#define GENIE2_GET_TEXT2(_lan,_uuid) _IFACE2_GET_TEXT_USINGUUID(_lan,_uuid)
#define GENIE2_VARIANT_TO_STRING(_lan,_uuid) __IFACE2_VARIANT_TO_STRING_OF_UUID(_lan,_uuid)

//subscrib/notify message,refer to _SUBSCRIBED_SUB_CMDS at interface2_cmds.h
#define GENIE2_SUBSCRIB_MESSAGE(_msg) __IFACE2_HANDLER_CMD(HCMD_SUBSCRIB_MESSAGE,_msg)
#define GENIE2_NOTIFY_MESSAGE(_msg,_paras) __IFACE2_NOTIFY_MESSAGE(_msg,_paras)

//read/write ini files
#define GENIE2_GET_SETTING(_key) __IFACE2_GET_SETTINGS(_key)
#define GENIE2_SET_SETTING(_key,_value) __IFACE2_SET_SETTINGS(_key,_value)

#define GENIE2_STATISTICS(_data) __IFACE2_STATISTICS__(_data,1,0)
#define GENIE2_STATISTICS_SEARCH(_data) __IFACE2_STATISTICS__(_data,2,0)
#define GENIE2_STATISTICS_MAXVALUE(_data,_max)  __IFACE2_STATISTICS__(_data,3,_max)
//#define GENIE2_STATISTICS2(_data,_num) __IFACE2_STATISTICS__(_data,_num)

//redirect resource directory
#define GENIE2_RES(_res) __IFACE2_RESOURCE__(_res)

#define GENIE2_LOG(_log) __IFACE2_LOG_(_log)

#define GENIE2_SET_FOCUS(_w) __IFACE2_SET_FOCUS__(_w)



//#define __IFACE2_TRANSLATE_LABEL_WITHMODE(_lab,_lan,_mode) __IFACE2_TRANSLATE_(_lab,HCMD_TRANSLATE_LABEL,_lan,_mode)
//#define TRANSLATE_LABEL(lab,lan) TRANSLATE_LABEL_WITHMODE(lab,lan,QString(""))

//#define TRANSLATE_BUTTON_WITHMODE(but,lan,mode) __IFACE2_TRANSLATE_(but,HCMD_TRANSLATE_BUTTON,lan,mode)
//#define TRANSLATE_BUTTON(but,lan) TRANSLATE_BUTTON_WITHMODE(but,lan,QString(""))



//#define GET_TEXT(lan) GET_TEXT_USINGUUID(lan,QGenieIface2ShareObject::sUuidIdx)


//#define VARIANT_TO_STRING(x) VARIANT_TO_STRING_OF_UUID(x,QGenieIface2ShareObject::sUuidIdx)



//#define GET_TEXT(lan) GET_TEXT_USINGUUID(lan,QGenieIface2ShareObject::sUuidIdx)
//for translation end


//#define NOTIFY_MESSAGE(msg,paras) HANDLER_CMD(HCMD_NO)

//    do{if(QGenieIface2ShareObject::sSingleInstance &&QGenieIface2ShareObject::sSingleInstance->mHandler )\
//    {QGenieIface2ShareObject::sSingleInstance->mHandler->handler_command(HCMD_SUBSCRIB_MESSAGE,msg);}}while(0)
//#ifdef Q_OS_WIN32
//#define NETGEAR_DIR (QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/")
#define NETGEAR_DIR (QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/")
//#else
//#define NETGEAR_DIR (QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.NETGEARGenie/")/*(QDir::home().absolutePath()+QString("/.NETGEARGenie/"))*/
//#endif



#endif // INTERFACE2_MACROS_H
