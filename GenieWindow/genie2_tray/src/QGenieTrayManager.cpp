#include "QGenieTrayManager.h"
#include "QGenieTrayIco_In.h"
#include "QGenieTrayIco_Out.h"
//#include "genie2_interface.h"
//#include "language_Ui.h"
#include <QUiLoader>


#define TRAY_MESSAGE_TITLE WINDOW_TITLE

#define MESSAGE_SHOWING_TIME 3000
#define LIST_MAX_SIZE 4


QGenieTrayManager::QGenieTrayManager(int type)
    :mMainWnd(NULL)
    ,mTrayIco(NULL)
{
    if(type==1)
    {
        mTrayIco=new QGenieTrayIco_In(this);
    }
    else if(type==2)
    {
        mTrayIco=new QGenieTrayIco_Out(this);
    }
}

QGenieTrayManager::~QGenieTrayManager()
{
    delete (mTrayIco);
}


void QGenieTrayManager::uiCommand(int cmd,const QString &para)
{
    if(mTrayIco)
    {
        mTrayIco->uiCommand(cmd,para);
    }
}
void QGenieTrayManager::_startUpdate()
{
#if 0
    qDebug() << "QGenieFrameTitle::slot_startUpdate()";
    QGenieLaunchInf *launchinf=QGenieLaunchInf::GetSingletonPtr();
    Q_ASSERT(launchinf);
    QGenieInterface2LanguageManager *lanm2=QGenieInterface2LanguageManager::GetSingletonPtr();
    Q_ASSERT(lanm2);




    ILaunchManager *manager=QGenieLaunchInf::sLaunchManager;
    Q_ASSERT(manager);



    bool mustretart=manager->startUpdate(UR_USER_CHOISE,lanm2->currentLanShortStr()
 #ifdef SHEET_STYLE_FORMAC
                                         ,mMainWnd
  #endif
                                         );
#endif
}


