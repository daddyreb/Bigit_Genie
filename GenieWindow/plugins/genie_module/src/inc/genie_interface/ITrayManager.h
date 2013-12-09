#ifndef ITRAYMANAGER_H
#define ITRAYMANAGER_H
#include <QString>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QUuid>
class IPluginInf;
#include "IPluginInf.h"



//control the tray icon
enum MOVIE_IDX{
    MI_REPAIR=0,
    MI_NUM
};

enum E_TRAY_STATUS
{
    ETS_GOOD,
    ETS_ERROR,
    ETS_OVERFLOW

};

class ITrayManager
{
public:
        virtual void setMessage(QString textTitle,QString textStr,QSystemTrayIcon::MessageIcon ico)=0;
        virtual void setAction(QAction *act,QUuid uuid)=0;
        virtual void addMenu(QMenu *menu,QUuid uuid)=0;

	virtual void hideWindow()=0;
	virtual void showWindow()=0;
	virtual void exitApp()=0;
        virtual void setMessageFlag(IPluginInf *pi,bool b=true)=0;//when plugin want the main frame notify it when tray message,call this
        virtual void setTrayStateIcon(E_TRAY_STATUS state,QString selficon="")=0;
        virtual void startMovie(MOVIE_IDX idx=MI_REPAIR,int msec=0)=0; //msec=0 means always loop 
        virtual void stopMovie()=0;
};

#endif
