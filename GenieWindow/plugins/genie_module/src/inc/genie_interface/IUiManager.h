#ifndef IINTERFACEMANAGER_H
#define IINTERFACEMANAGER_H


//some interface to control the main frame ui
#define UI_CMD_SET_ROUTE "ui_cmd_setroute"
#define UI_CMD_SET_WIFI "ui_cmd_setwifi"
#define UI_CMD_SET_FRIMWARE "ui_cmd_setfirmware"
#define UI_CMD_SHOW_UPDATE_BUTTON "ui_cmd_showupbut"
#define UI_CMD_HIDE_UPDATE_BUTTON "ui_cmd_hideupbut"
#define UI_CMD_CHANGE_CHANNEL "ui_cmd_changechannel"
#define UI_CMD_TRURNTO_GACC "ui_cmd_togacc"
#define UI_CMD_DIALOG "ui_cmd_dlg"
#define UI_CMD_SHOWNORMAL "ui_cmd_snm"

#include "IPluginInf.h"
#include <QUuid>

enum NET_STATE{
    NS_OK=0,  
    NS_ERROR,
    NS_OVERFLOW
};



class IPluginInf;

class IUiManager
{
public:
        virtual void setWaitState(IPluginInf *pi,QString resion)=0;
        virtual void restorNormalState(IPluginInf *pi,QString resion)=0;
        virtual void changeStackIdx(int idx)=0;
        virtual int command(QString cmd,QString para)=0;

       // virtual QClipboard *getClipBoard()=0;
        virtual QString getClipBoardString()=0;
        virtual void setClipBoardString(const QString s)=0;

        virtual void setBigButtonIcon(const QString &ico,QUuid uuid,int idx=0)=0;
        virtual void setBigButtonUserStateText(const QString &stylesheet,const QUuid &uuid,int idx=0)=0;
        virtual void setBigButtonUserStateBKGround(const QString &stylesheet,const QUuid &uuid,int idx=0)=0;

        /*
          idx means the idx of bigbutton ,if the plugin have more than one bigbutton
        */
//        virtual void setBigButtonState(IPluginInf *pi,ENUM_PLUGIN_STATUS state,int idx=0)=0;
        virtual void setBigButtonSubState(IPluginInf *pi,ENUM_BIGBT_SUB_STATE state,int idx=0)=0;
        virtual void setNetStateFlag(QUuid uuid,bool setit=true)=0;
        virtual void setRouteTypeFlag(QUuid uuid,bool setit=true)=0;
        virtual void setGatewayFlag(QUuid uuid,bool setit=true)=0;

        virtual void setBackGroundImg(QUuid uuid,QString img,QString para="")=0;

#ifdef USE_CHECK_ADMIN
        virtual bool checkAdmin()=0;
#endif

        virtual void setLeftButtonChecked(IPluginInf *pi,int idx = 0) = 0;

};

#endif
