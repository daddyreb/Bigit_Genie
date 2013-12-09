#ifndef IINFOMANAGER_H
#define IINFOMANAGER_H
#include <QString>
#include <QIcon>
#include <QRect>
#ifdef INFO_WRITE_BACK
#include <QUuid>
#endif
#include "info_defs.h"



class IInfoManager
{
public:
        virtual QIcon  getIcon()=0; //main frame may have a global icon
#ifdef INFO_WRITE_BACK
        virtual QString getInfo(QString what,QUuid uuid=QUuid())=0;	/*return a string include version number and something else*/
#else
        virtual QString getInfo(int what,int para=0)=0;	/*return a string include version number and something else*/
#endif

        virtual const QRect &getGeometry(int what)=0;
#ifdef INFO_WRITE_BACK
        virtual void notifyInfo(QString type,QString result)=0;
#endif
        virtual void *getSharedMemory(int para)=0;
        virtual void notifyPluginMessage(int msg,QString para0="",QString para1="")=0;

};
#endif
