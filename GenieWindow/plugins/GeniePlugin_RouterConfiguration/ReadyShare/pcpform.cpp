#include "pcpform.h"
#include <QtCore>
#include <QtGui>

#include "QGenieRCWirelessObject.h"
#include "genie2_interface.h"

///////////
#include <QHostInfo>

_HostInfoHelper::_HostInfoHelper()
{

};

_HostInfoHelper::~_HostInfoHelper()
{
    wait();
}

void _HostInfoHelper::getDNS(const QString &path)
{
    mMutex.lock();
    mPath=path;
    mMutex.unlock();
    start();
}

QHostInfo _HostInfoHelper::getHostInfo()
{
    mMutex.lock();
    QHostInfo ret=mHostInfo;
    mMutex.unlock();
    return ret;
}

void _HostInfoHelper::run()
{
    mMutex.lock();
    QString path=mPath;
    mMutex.unlock();

    QHostInfo hinfo;
    hinfo=QHostInfo::fromName(path);

    mMutex.lock();
    mHostInfo=hinfo;
    mMutex.unlock();

}


PCPForm::PCPForm(QWidget *parent)
    :QWidget(parent)
    ,mManagerObj(NULL)
    ,iIpChangedCount(0)
    ,mBasicObj(NULL)
    ,mRemoteObj(NULL)
    ,mPrintObj(NULL)
    //    ,widgetUnregister(NULL)
    ,mStack(NULL)
{
   // setupUi0();
    indexOfStackLayout=0;
    mHostInfoHelper=new _HostInfoHelper();
    connect(mHostInfoHelper,SIGNAL(finished()),this,SLOT(routerlogin_netDNS()));
}

PCPForm::~PCPForm()
{
    GENIE2_SAFEDELETE(mHostInfoHelper);
    GENIE2_SAFEDELETE(mManagerObj);
    GENIE2_SAFEDELETE(mBasicObj);
    GENIE2_SAFEDELETE(mRemoteObj);
    GENIE2_SAFEDELETE(mPrintObj);

    //    GENIE2_SAFEDELETE(widgetUnregister);
}

void PCPForm::init()
{
    const QString strip=Genie_IP();
    if (strip.length()<6)
    {
        return;
    }
    mBasicObj->ipChanged(strip);

    mManagerObj->init(mFlags);
    //    int nMask = mFlags;

    //    readyShareManager->hideAndDisRadio();


    indexOfStackLayout = 0;

}

const QString &PCPForm::Genie_IP()
{
    return mRouterlogin_netIp;
}

void PCPForm::setupUi0()
{
    //----create up widget and show information-------------------
    //insert all widgets into a stacked layout

    mStack=new QStackedLayout(this);
    mBasicObj=new QGenieRSBasic();
    mRemoteObj=new QGenieRSRemote();
    mPrintObj=new QGenieRSPrint();
    //    widgetUnregister=new QGeniePlugin_Remote_Unregister_ReadyShare();

    /////////////////
    mManagerObj = new QGenieRSManager();
    mStack->addWidget(mManagerObj->getUi());

    mStack->addWidget(mBasicObj->getUi());


}

void PCPForm::setupUi1()
{

    mStack->addWidget(mRemoteObj->getUi());
    mStack->addWidget(mPrintObj->getUi());

    mStack->setCurrentIndex(0);
    //////////////////
    mManagerObj->mPcpForm = this;
}

//void PCPForm::RegisterNotYet()
//{
//    widgetRemoteChoosed->strRegUser="";
//    widgetRemoteChoosed->strRegPwd="";
//    widgetRemoteChoosed->mTxtPwd->setText("");
//    widgetRemoteChoosed->mTxtUser->setText("");
//    mStack->setCurrentIndex(2);
//}

void PCPForm::routerlogin_netDNS()
{
    QHostInfo host=mHostInfoHelper->getHostInfo();
    QString ip;
    if ( host.error() == QHostInfo::NoError && !host.addresses().isEmpty())
    {
		foreach(QHostAddress address, host.addresses())
		{
			if(address.toIPv4Address())
			{
				ip = address.toString().trimmed();
				break;
			}else
			{
				continue;
			}
		}
       /// ip=host.addresses()[0].toString().trimmed();
    }
    if(!ip.isEmpty()&& ip !=mRouterlogin_netIp)
    {
        if(mRouterlogin_netIp.isEmpty())
        {
            mRouterlogin_netIp=ip;
            init();
        }
        else
        {
            QGenieRCWirelessObject *wobj=QGenieRCWirelessObject::GetSingletonPtr();
            Q_ASSERT(wobj);
            wobj->logOut(false);
        }
    }
}

//void PCPForm::UnRegisterOk()
//{
//    if (widgetRemoteChoosed)
//    {
//        widgetRemoteChoosed->strRegUser="";
//        widgetRemoteChoosed->strRegPwd="";
//        widgetRemoteChoosed->mTxtPwd->setText("");
//        widgetRemoteChoosed->mTxtUser->setText("");
//        mStack->setCurrentIndex(2);
//    }
//}

//void PCPForm::RegisterOk(QString strUser,QString strPassword)
//{
//    if (widgetUnregister)
//    {
//        widgetUnregister->strRegUser=strUser;
//        widgetUnregister->strRegPwd=strPassword;

//        //todo
////        widgetUnregister->lblUserName->setText(strUser);
//        mStack->setCurrentIndex(4);
//    }
//}

//void PCPForm::slotPreIndex(int iPreIndex)
//{
//    iPreIdx=iPreIndex;
//}

void PCPForm::showPreFrom()
{
    mStack->setCurrentIndex(0);
}




//void PCPForm::changeEvent(QEvent *e)
//{
//    mManagerObj->changeEvent(e);
//    mBasicObj->changeEvent(e);
//    mRemoteObj->changeEvent(e);
//    //    widgetUnregister->changeEvent(e);
//}

/////////////////////////////////////////////////////
bool PCPForm::getPath(QString &exePath)
{
    //¥”HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\GenieTimeline.exe»°÷µ
    const QString strPath = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\GenieTimeline.exe";
    QString tmp;
    QSettings setting(strPath, QSettings::NativeFormat);

    exePath = setting.value("Default").toString();

    qDebug() << "PCPForm::getPath():  " << exePath;

    if(exePath.isEmpty())
    {
        return false;
    }

    return true;
}

void PCPForm::whenLogin(const QString &username, const QString &password)
{
    mUsername=username;
    mPassword=password;
    mRouterlogin_netIp="";
    mHostInfoHelper->getDNS("routerlogin.net");
 //   QHostInfo::lookupHost(,this,SLOT(routerlogin_netDNS(QHostInfo)));
    mManagerObj->init(0);
    mStack->setCurrentWidget(mManagerObj->getUi());
    mRemoteObj->showError(-1);
}

void PCPForm::runExe()
{
    qDebug() << "PCPForm::runExe()";

    bool bRet = false;
    QString strPath;


    bRet = getPath(strPath);
    if(bRet)
    {
        qDebug()<<"OKs";
        mProcess.startDetached(strPath, QStringList());
    }
    else
    {
        QDesktopServices::openUrl(QUrl(GENIE2_GET_SETTING(RSVALT_LINK).toString()));
    }
}
