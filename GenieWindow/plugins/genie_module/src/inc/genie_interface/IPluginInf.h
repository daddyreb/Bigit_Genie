#ifndef IPLUGININF_H
#define IPLUGININF_H
#include  <QString>
#include <QStringList>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QList>
#include "info_defs.h"
#include "cmd_defs.h"

class IManagerHandler;
//enum ENUM_PLUGIN_STATUS
//{
//	EPS_GOOD=0,
//	EPS_ERROR,
//        EPS_NUM
//};

enum ENUM_BIGBT_SUB_STATE
{
    EBSS_NONE,
    EBSS_BUT,
    EBSS_REFRESH,
    EBSS_NUM
};

enum MAIN_FRAME_RESPONSE
{
    MFR_MINIMIZE,
    MFR_MAXIMIZE,
    MFR_NORMALIZE,
    MFR_HIDE,
    MFR_UNKNOWN
};





#define WAIT_STRING "_wait..."

class IPluginInf
{
public:
    virtual QString text()=0;
	virtual ~IPluginInf(){
	};
    virtual QWidget* widget(QWidget *parent)=0;

    virtual QStringList bigButtonParas()=0;
    /*
      <notifystring>:<priority>:para1=value1:para2=value2
    */


    virtual void notifyBigButton(QString s)=0;

    virtual QIcon * icon()=0;
    virtual QString  info()=0;
	/*
		"UUID:{7a7ae6e2-a85f-4586-8a80-fd3ee9a7ed39};BIND:1;HAVEWND:1;VERSION:XX.XX.XXXX"
		beware use self uuid
	*/

    virtual int priority()=0;			//normal plugin return 3,0 is the highest
    virtual int changelanguage(int l)=0;
    virtual void setHandler(IManagerHandler *hdl)=0; //plugin can save the controller,when
										//registed to main frame,then can use it to control the main frame
										//refer to IClickManager,IInfoManager,IManagerHandler,IToyManager,ITrayManager......
    virtual void startup(bool mini)=0; 			//windowless plugin use this
	virtual void notifyRaise()=0;		//when plugin raise to top of stack,plugin may catch it to start/stop some processes
	virtual void notifyLower()=0;		//opposite to notifyRaise
//        virtual int getStatus(QString notifyPara)=0;			//return the status of plugin,refer to ENUM_PLUGIN_STATUS
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)=0;
                                                //mainframe_response
                                                //0.normalize
                                                //1.hide
                                                //2.minimize
                                                //3.maximize

    virtual QString getInfo(int para)=0;
#ifdef INFO_WRITE_BACK
    virtual void notifyInfo(QString request,QString result)=0;
#endif
   virtual void command(int cmd,QString para0="",QString para1="")=0;
   virtual void command(int cmd, int para)=0;

   virtual void *getPoint(int para)=0;

};
Q_DECLARE_INTERFACE(IPluginInf,"com.dragonflow.GeniePlugin.IPluginInf/1.0")
#endif // IPluginInf_H
