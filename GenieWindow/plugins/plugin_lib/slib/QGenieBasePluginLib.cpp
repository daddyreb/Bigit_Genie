#include "QGenieBasePluginLib.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include <QAction>
#include "ITrayManager.h"
//#include "version.h"
#include "bbt_paras.h"
#include "IUiManager.h"

#include "uuid.h"

#define PLUGIN_INFO "UUID:"UUID_OF_THIS";\
BIND:0;\
            HAVEWND:%1;\
                VERSION:%2"

IManagerHandler *sHandler=NULL;

QGenieBasePluginLib * sSlib=NULL;
QGenieBasePluginLib::QGenieBasePluginLib()
    :mHaveWnd(false)
    ,mVersionStr("0.0.0.0")
    ,mW(NULL)
    ,mPriority(9999999)
    ,mName("noname")
    ,mBkImg("temp")
    ,mSelfDeleteW(true)
{

    Q_ASSERT(sSlib==NULL);
    sSlib=this;
}

QGenieBasePluginLib::~QGenieBasePluginLib()
{
    if(mSelfDeleteW && mW && !mW->parent())
    {
        delete mW;
    }
}

QString QGenieBasePluginLib::text()
{
    return QString("NotifyLeft:%1").arg(mName);
}

QWidget* QGenieBasePluginLib::widget(QWidget *parent)
{
    if(mW)
    {
        mW->setParent(parent);
        mSelfDeleteW=false;
    }
    return mW;
}

QIcon * QGenieBasePluginLib::icon()
{
    return NULL;
}

int QGenieBasePluginLib::changelanguage(int idx)
{
	return 0;
}

QString  QGenieBasePluginLib::info()
{
    return QString(PLUGIN_INFO).arg(mHaveWnd?1:0).arg(mVersionStr);
}

QString QGenieBasePluginLib::getInfo(int para)
{
    if(PLUGIN_INF_LANGUAGE_FILE == para)
    {
        return mLanFname;
    }
    return "";
}

int QGenieBasePluginLib::priority()
{
    return mPriority;
}


void QGenieBasePluginLib::setHandler(IManagerHandler *hdl)
{
    sHandler=hdl;

}

void QGenieBasePluginLib::startup(bool mini)
{
    Q_ASSERT(sHandler);
    if(mStyleCache!="")
    {
        setBigbuttonText(mStyleCache);
    }
}

QStringList QGenieBasePluginLib::bigButtonParas()
{

    QStringList ret;
    QString stemp="%1:%2:"BBT_PARA_LAN_IDX"=%3";
    stemp=stemp.arg(mBkImg).arg(mPriority).arg(mName);
    ret <<stemp;
    return ret;
}

void QGenieBasePluginLib::notifyBigButton(QString s)
{

}

void QGenieBasePluginLib::notifyRaise()		//when plugin raise to top of stack,plugin may catch it to start/stop some processes
{

}
void QGenieBasePluginLib::notifyLower()		//opposite to notifyRaise
{

}


void QGenieBasePluginLib::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{

}

void QGenieBasePluginLib::command(int cmd,QString para0,QString para1)
{

}

void QGenieBasePluginLib::command(int cmd, int para)
{

}

void *QGenieBasePluginLib::getPoint(int para)
{
	return NULL;
}

void QGenieBasePluginLib::setBigbuttonText(const QString &s)
{
    if(!sHandler)
    {
        mStyleCache=s;
    }
    else
    {
        sHandler->getUiManager()->setBigButtonUserStateText(s,UUID_OF_THIS);
        mStyleCache="";
    }
}

//Q_EXPORT_PLUGIN2(slib, slib)



