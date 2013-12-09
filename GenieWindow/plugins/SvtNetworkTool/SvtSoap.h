#ifndef SVTSOAP_H
#define SVTSOAP_H

#include <QtCore>
#include "SvtNetworkTool.h"

#define RCODE_TAG "ResponseCode"
    bool isSetApi(const QString &module,const QString &method);
class SVT_NETWORKTOOL_EXPORT SVT_Soap
{
public:
    explicit SVT_Soap(bool *exitFlag=NULL);
    void setExitFlag(bool *para);
//    void setHost(const QString &host);
    void setAutoChangePort(bool b);

    bool _doFunc(const QString &host,const QString &module,const QString &method,quint32 port
                ,const QMap<QString,QString> *paras=NULL,QMap<QString,QString> *retmap=NULL);
    bool doFunc(const char *host,const char *module,const char *method
                ,const QMap<QString,QString> *paras=NULL,QMap<QString,QString> *retmap=NULL);
    bool doFunc(const QString &host,const QString &module,const QString &method
            ,const QMap<QString,QString> *paras=NULL,QMap<QString,QString> *retmap=NULL);

    void setRouteType(const QString &routetype);
    void sendStart(const QString &host,quint16 port);
    void sendFinish(const QString &host,quint16 port);
protected:

    bool *mExitFlag;
//    QString mHost;
//    QString mRouteType;
    enum _InnerFlags
    {
        IS_CGDG=1<<0,
        IS_5000PORT=1<<1
    };
    quint32 mFlags;
    bool mAutoChangePort;

    int getRetValue(const QByteArray &retarray);

};

#endif // SVTSOAP_H
