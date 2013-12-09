#include "stable.h"
QGenieSplashManager::QGenieSplashManager()
    :mSplash(NULL)
{
//    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_updateImage()));
}

QGenieSplashManager::~QGenieSplashManager()
{
    GENIE2_SAFEDELETE(mSplash);
}

//void QGenieSplashManager::setRouterModel(const QString &model)
//{
//    if(model.isEmpty())
//    {
//        return;
//    }
//    QTimer::singleShot(qrand()%3000,this,SLOT(slot_updateImage()));
//}


//void QGenieSplashManager::slot_updateImage()
//{
//    QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();
//    QImageNameGengerator gener(model);
//    QPixmap router_pixmap(gener.getImageName(IE_B31));
//    QString fname=QString(NETGEAR_DIR)+"splash.png";
//    QImage splash_img(fname);
//    QPainter p(&splash_img);
//    p.drawPixmap(0,0,router_pixmap);
//    splash_img.save(fname);
//}

void QGenieSplashManager::showSplash()
{
    QString fname=QString(NETGEAR_DIR)+"splash.png";
    if(!QFile::exists(fname))
    {
        fname=":/splash.png";
    }
    mSplash=new QSplashScreen();
    mSplash->setPixmap(QPixmap(fname));
    mSplash->show();
    mSplash->raise();
#ifdef SHOW_VERSIONNUMBER_IN_SPLASH
//    mSplash->setStyleSheet("font-family:\"Arial\"");

    QString t=VERSION_STRING;
    QStringList templist=t.split(".");
    t="";
    int c=templist.count();
    for(int i=0;i<c;i++)
    {
        bool btemp;
        int itemp=templist[i].toInt(&btemp);
        t=t+QString((t.isEmpty())?"":".")+(btemp?QString::number(itemp):templist[i]);
    }
    mSplash->showMessage(QString("Version: %1").arg(t),Qt::AlignBottom|Qt::AlignHCenter);
#endif
}

void QGenieSplashManager::hideSplash()
{
    if(mSplash)
    {
        mSplash->finish(NULL);
        mSplash->deleteLater();
        mSplash=NULL;
    }
}

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

QGenieLaunchInf::QGenieLaunchInf()
    :mAllSingleClasses(NULL)
    ,mSplashM(new QGenieSplashManager())
{
 
    if(qApp->arguments()[0].contains("genieVersionDetector"))
    {
        return;
    }
    if(qApp->arguments().contains("-nsis"))
    {
        return;
    }


    mAllSingleClasses=new QGenieAllSingletonClasses();
    if(mAllSingleClasses->mInterface2Helper->mbConflictWithOtherProcess)
    {
        this->setProperty("exitflag",true);
        return;
    }



    QGenieInterface2Helper *i2helper=QGenieInterface2Helper::GetSingletonPtr();
    Q_ASSERT(i2helper);
    i2helper->loadAllPlugins();

    QGenieInterface2LanguageManager *lanm2=QGenieInterface2LanguageManager::GetSingletonPtr();
    Q_ASSERT(lanm2);
    lanm2->slot_setLanguage(-1);

    mSplashM->hideSplash();
}

QGenieLaunchInf::~QGenieLaunchInf()
{
    GENIE2_SAFEDELETE(mSplashM);
    GENIE2_SAFEDELETE(mAllSingleClasses);
}

//void QGenieLaunchInf::showSplash()
//{
//    mSplash=new QSplashScreen();
//    mSplash->setPixmap(QPixmap(":/splash.png"));
//    mSplash->show();
//    mSplash->raise();
//    mSplash->setStyleSheet("font-family:\"Arial\"");

//    QString t=VERSION_STRING;
//    QStringList templist=t.split(".");
//    t="";
//    int c=templist.count();
//    for(int i=0;i<c;i++)
//    {
//        bool btemp;
//        int itemp=templist[i].toInt(&btemp);
//        t=t+QString((t.isEmpty())?"":".")+(btemp?QString::number(itemp):templist[i]);
//    }
//    mSplash->showMessage(QString("Version: %1").arg(t),Qt::AlignBottom|Qt::AlignHCenter);

//}





QVariant QGenieLaunchInf::command(int cmd,const QVariant &paras)
{
    Q_UNUSED(paras)
    if(cmd ==I2CMD_UUID)
    {
        return QString(FRAME_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
//        qDebug()<<"QGenieLaunchInf::command():"<<QString(VERSION_STRING);//yankai test
        return QString(VERSION_STRING);
    }
    else if(cmd == I2CMD_LAYZYLOAD)
    {
        static bool first=true;
        if(first)
        {
            QGenieInterface2Helper *i2helper=QGenieInterface2Helper::GetSingletonPtr();
            Q_ASSERT(i2helper);
            i2helper->loadPlugin(PLUGIN_FILE_UPDATE);
            first=false;
            return true;
        }
        return false;
    }

    else if(cmd==I2CMD_ABOUTTO_QUIT)
    {

    }

    return QVariant();
}




