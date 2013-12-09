#ifndef slib_H
#define slib_H

#include <QtCore>
#include "./genie_interface/IPluginInf.h"
#include <QSystemTrayIcon>


class QGenieBasePluginLib : public QObject,public IPluginInf
{
    Q_OBJECT
//    Q_INTERFACES(IPluginInf)
public:
    QGenieBasePluginLib();
    virtual ~QGenieBasePluginLib();
private:
    virtual QString text();
    virtual QWidget* widget(QWidget *parent);

    virtual QIcon  *icon();
    virtual QString  info();
    virtual QString getInfo(int para);
    virtual int priority();
    virtual int changelanguage(int idx);
    virtual void setHandler(IManagerHandler *hdl);
    virtual void startup(bool mini);
    virtual QStringList bigButtonParas();
    virtual void notifyBigButton(QString s);

    virtual void notifyRaise();		//when plugin raise to top of stack,plugin may catch it to start/stop some processes
    virtual void notifyLower();		//opposite to notifyRaise
//    virtual int getStatus(QString notifyPara);		//return the status of plugin,refer to ENUM_PLUGIN_STATUS
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);

    virtual void command(int cmd,QString para0="",QString para1="");
    virtual void command(int cmd, int para);

    virtual void *getPoint(int para);

    bool mHaveWnd;
    QString mVersionStr;
    QWidget *mW;
    QString mLanFname;
    int mPriority;
    QString mName;
    QString mBkImg;
    QString mStyleCache;
    bool mSelfDeleteW;
 public:
    inline void setUiEnable(bool enable=true){mHaveWnd=enable;};
    inline void setVersion(const QString &v){
        QString stemp=v;
        QStringList templst=v.split(".");
        if(templst.size()>=4)
        {
            mVersionStr=stemp.sprintf("%02d.%02d.%02d.%02d",
                                         templst[0].toInt(),
                                         templst[1].toInt(),
                                         templst[2].toInt(),
                                         templst[3].toInt()
                                         );
        }
    };
    inline void setWidget(QWidget *w){mW=w;};
    inline void setLanFname(const QString &fname){mLanFname=fname;};
    inline void setPriority(int pri){mPriority=pri;};
    inline void setPluginName(const QString &name){mName=name;};
    inline void setPluginNameLanIdx(int idx){mName=QString::number(idx);};
    inline void setBigbuttonImgName(const QString &img){mBkImg=img;};
    void setBigbuttonText(const QString &s);


};

#endif // slib_H
