#include "QGenieUiObject.h"
#include "GeniePlugin_Ui.h"
#include <QtGui>
#include <QUiLoader>
#include <QLocale>

//#include <QAbstractItemModel>
#include "QGenieButtonBig.h"
#include "language_Ui.h"
#include "QGenieAdvStateMachine.h"
#include "genie2_tray.h"
#include "genie2_waterwindow.h"
#include "QGenieTitleUpdate.h"
#include "QGenieModalDialog.h"
#include "genie2_routeranim.h"


#define TITLE_WIFI_TOOLTIP_MODE "<span style=\" color:rgb(0,113,215);\"><b>%1:</b></span> %2"
#define TITLE_WIFI_TEXT_MODE "<span style=\" color:rgb(0,0,0);\"><b>%1:</b></span> <span style=\" color:#4AC600;\"><b>%2</b></span>"///kai.yan modify color
//#define BIGBUTTON_UI "X:\\Genie1.2\\trunk\\testplugin_ui\\ui\\QGenieButtonBig.ui"
#define WAIT_TRAYICON_RET (5 * 1000)

_tLinkData::_tLinkData()
{
    leftBut=NULL;
    bbt=NULL;
    stackW=NULL;
    uiPara[UP_BUTLINE_MODE]=0;
    uiPara[UP_BUTLINE_PARA1]=-L_UI_CLICK_HERE;
    uiPara[UP_BUTLINE_PARA2]=0;
};

class _tMoveData
{
public:
    _tMoveData(){mDrag=false;};
    bool mDrag;
    QPoint mDragPoint;
};
#ifdef Q_OS_MACX
class _CMouseArrowController:public QObject
{
public:
    _CMouseArrowController(){};
    bool eventFilter(QObject *o, QEvent *e)
    {
    QEvent::Type type=e->type();
        QWidget *w=qobject_cast<QWidget *>(o);
        Q_ASSERT(w);

        if(type==QEvent::Enter)
        {
            qApp->setOverrideCursor(Qt::PointingHandCursor);
        }
        else if(type==QEvent::Leave)
        {
            qApp->setOverrideCursor(Qt::ArrowCursor);
        }
	return false;
    };
};
static _CMouseArrowController sMouseArrowcontroller;
#endif

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#ifdef USE_DIRTY_FOCUS_CLEARER
QGenieDirtyFocusFrameClearer::QGenieDirtyFocusFrameClearer()
    :mFocusW(NULL)
    ,mState(-1)
{
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_TimeOut()));
}

QGenieDirtyFocusFrameClearer::~QGenieDirtyFocusFrameClearer()
{
    mTimer.stop();
    disconnect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_TimeOut()));
    if(mFocusW)
    {
        disconnect(mFocusW,SIGNAL(destroyed(QObject*)),this,SLOT(slot_WidgetDistroyed(QObject*)));
    }
}

void QGenieDirtyFocusFrameClearer::startRefresh()
{
    if(!mTimer.isActive())
    {
        mState=S_GETWINDOW;
        mTimer.start(10);
    }
}

void QGenieDirtyFocusFrameClearer::slot_TimeOut()
{
    QWidget *w;
    switch(mState++)
    {
    case S_GETWINDOW:
        w=qApp->focusWidget();
        if(!w)
        {
            stop();
        }
        else
        {
            mFocusW=w;
            w->clearFocus();
            connect(w,SIGNAL(destroyed(QObject*)),this,SLOT(slot_WidgetDistroyed(QObject*)));
        }
        break;
    case S_SETFOCUS_BACK:
        w=qApp->focusWidget();
        if(!w && mFocusW)
        {
            mFocusW->setFocus();
        }

        break;
    default:
        stop();
    }
}

void QGenieDirtyFocusFrameClearer::stop()
{
    if(mFocusW)
    {
        disconnect(mFocusW,SIGNAL(destroyed(QObject*)),this,SLOT(slot_WidgetDistroyed(QObject*)));
    }
    mTimer.stop();
}

void QGenieDirtyFocusFrameClearer::slot_WidgetDistroyed(QObject *o)
{
    mTimer.stop();
    mFocusW=NULL;
}
#endif
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////





QGenieUiObject::QGenieUiObject(QObject *parent)
    :QObject(parent)
    ,mMainFrame(NULL)
    ,mStackWidget(NULL)
    ,mLeftPan(NULL)
    ,mHomeBut(NULL)
    ,mHomePage(NULL)
    ,mLanComb(NULL)
//    ,mAdvW(NULL)
//   ,mAdvStateMachine(NULL)
//    ,mSearchEdt(NULL)
//    ,mGo(NULL)
    ,mTitleUpdate(NULL)
    ,mLanSel(NULL)
    #ifdef Q_OS_WIN32
    ,mTrayManager(new QGenieTrayManager(2))
    #else
    ,mTrayManager(new QGenieTrayManager(1))
    #endif
    ,mCurrentLinkData(NULL)
    #ifdef USE_DBG_STACK
    ,mDbgStack(NULL)
    ,mSavedData(0)
    #endif
{

//  mTitleBut = NULL;
    mUpdateBut = NULL;
    mCurLink="www.netgear.com/genie";

    mTitleUpdate = new QGenieTitleUpdate(this);
    connect(mTitleUpdate, SIGNAL(updateIcon()), this, SLOT(slot_updateIcon()));

#ifdef GENIE2_TRAY
    connect(mTrayManager,SIGNAL(actionTrigged(QString)),this,SLOT(slot_trayActionTrigged(QString)));
    connect(mTrayManager,SIGNAL(canInitNow()),this,SLOT(slot_InitTray()));
#endif
    mUrlTimer.setSingleShot(true);
    if(QApplication::arguments().contains("-dbg"))
    {
        _doDBG();
    }
}

QGenieUiObject::~QGenieUiObject()
{
    GENIE2_SAFEDELETE(mTrayManager);
#ifdef USE_DBG_STACK
    GENIE2_SAFEDELETE(mDbgStack);
#endif

    //    GENIE2_SAFEDELETE(mStackWidget);
    //    GENIE2_SAFEDELETE(mMainFrame);
    //    GENIE2_SAFEDELETE(mAdvStateMachine) ;
    //can not delete mMainFrame,otherwise app will crush when exiting,but i don't know why
    //    if(mMainFrame)
    //    {
    //        delete mMainFrame;
    //    }
    QMap<QWidget *,_tMoveData *>::Iterator i=mMoveWidgetData.begin();
    while(i!=mMoveWidgetData.end())
    {
        delete (i.value());
        i++;
    }

    QMap<short,_tLinkData *>::Iterator i2=mLinkData.begin();
    while(i2!=mLinkData.end())
    {
        delete (i2.value());
        i2++;
    }
}

void QGenieUiObject::enableMove(QWidget *w)
{
    w->installEventFilter(this);
    connect(w,SIGNAL(destroyed(QObject *)),this,SLOT(slot_MovableWidgetDestroyed(QObject *)));
    if(!mMoveWidgetData.contains(w) || mMoveWidgetData[w]==NULL)
    {
        _tMoveData *data=new _tMoveData;
        mMoveWidgetData.insert(w,data);
    }
}

void QGenieUiObject::createMainFrame()
{
    //todo findchild youhua
    bool btemp;
    QUiLoader loader;
    QFile mainframe_file(GENIE2_RES("ui/QGenieMainUi.ui"));
    btemp=mainframe_file.open(QFile::ReadOnly);
    Q_ASSERT(btemp);
    mMainFrame=loader.load(&mainframe_file, NULL);
	
	QWidget *tilte = mMainFrame->findChild<QWidget *>("title");
	tilte->setStyleSheet("background-image: url(:/BIGIT/bigit/title-bg.png);");
    Q_ASSERT(mMainFrame);
    QGenieIface2ShareObject::sSingleInstance->mMainFrame=mMainFrame;
	
    mStackWidget=mMainFrame->findChild<QStackedWidget *>("stack");
    Q_ASSERT(mStackWidget);
	mStackWidget->setStyleSheet("background-image: url(:/BIGIT/bigit/stack_bg.png);");

    mLeftPan=mMainFrame->findChild<QWidget *>("leftpan");
    Q_ASSERT(mLeftPan);
	mLeftPan->setStyleSheet("background-image: url(:/BIGIT/bigit/left_pan_bg.png);");

	mBottomPan = mMainFrame->findChild<QWidget*>("but_w");
	Q_ASSERT(mBottomPan);
	mBottomPan->setStyleSheet("background-image: url(:/BIGIT/bigit/title-bg.png);");

	mRightMargin = mMainFrame->findChild<QWidget*>("right_margin");
	Q_ASSERT(mRightMargin);
	mRightMargin->setStyleSheet("background-image: url(:/BIGIT/bigit/title-bg.png);");

	mLeftMargin = mMainFrame->findChild<QWidget*>("left_margin");
	Q_ASSERT(mLeftMargin);
	mLeftMargin->setStyleSheet("background-image: url(:/BIGIT/bigit/title-bg.png);");

//   mAdvW=mMainFrame->findChild<QWidget *>("adv");
//    Q_ASSERT(mAdvW);
//    mAdvW->installEventFilter(this);

	Qt::WindowFlags flags = 0;
    flags |= Qt::WindowFullscreenButtonHint;

    mLeftPan->setWindowFlags(flags);
	mLeftPan->setFixedSize(180,500);
	mStackWidget->setWindowFlags(flags);
	mStackWidget->setFixedSize(685,500);

	mMainFrame->setWindowFlags(flags);
//	mAdvW->setWindowFlags(flags);
//	mAdvW->setFixedSize(517,85);
//  mAdvW->setStyleSheet(QString("background-image: url(%1);").arg(GENIE2_RES("images/fg_ad_frame.png")));

    mLanComb=mMainFrame->findChild<QComboBox *>("cob_sl_lan");
    Q_ASSERT(mLanComb);
    connect(mLanComb,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_LanChanged(int)));

//    mSearchEdt=mMainFrame->findChild<QLineEdit *>("search");
//    Q_ASSERT(mSearchEdt);
//    connect(mSearchEdt,SIGNAL(returnPressed()),this,SLOT(slot_StartSearch()));

//    mGo=mMainFrame->findChild<QPushButton *>("go");
//    Q_ASSERT(mGo);
//    connect(mGo,SIGNAL(pressed()),this,SLOT(slot_ButClicked()));
//    GENIE2_TRANSLATE_BUTTON(mGo,L_UI_SEARCH_GO);

    GENIE2_TRANSLATE_CHILD_BUTTON(mMainFrame,";about",L_UI_TITLE_ABOUT);

    QList<QPushButton *> bts=mMainFrame->findChildren<QPushButton *>();
    int c=bts.count();
    for(int i=0;i<c;i++)
    {
        connect(bts[i],SIGNAL(clicked()),this, SLOT(slot_ButClicked()),Qt::UniqueConnection);
    }

//	mGo->setWindowFlags(flags);
//	mSearchEdt->setWindowFlags(flags);

#ifdef Q_OS_MACX
    mMainFrame->setWindowFlags(Qt::Window
                               | Qt::WindowTitleHint
                               | Qt::WindowMinimizeButtonHint
                               |Qt::WindowCloseButtonHint
                               | Qt::CustomizeWindowHint);
#else
    mMainFrame->setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
#endif
    mMainFrame->setFixedSize(mMainFrame->geometry().size());
#ifdef Q_OS_WIN32
    enableMove(mMainFrame);
#endif

    slot_ReloadQss();


    mHomePage=mMainFrame->findChild<QWidget*>("home");
    Q_ASSERT(mHomePage);
	mHomePage->setStyleSheet("background-image: url(:/BIGIT/bigit/stack_bg.png);");
    QGridLayout *layout=qobject_cast<QGridLayout *>(mHomePage->layout());
    Q_ASSERT(layout);
    mainframe_file.close();


    for(int i=0;i<(sizeof(mBbts)/sizeof(mBbts[0]));i++)
    {
        mBbts[i]=new QGenieButtonBig(NULL);
        connect(mBbts[i],SIGNAL(clicked()),this,SLOT(slot_LinkButClicked()));
        layout->addWidget(mBbts[i],i/3,i%3);
    }
//    mAdvStateMachine=new QGenieAdvStateMachine();
//    connect(mAdvStateMachine,SIGNAL(changeADV(QString,QString)),this,SLOT(slot_ChangeAdvPic(QString,QString)));
//    mAdvStateMachine->start();


//    mTitleBut = mMainFrame->findChild<QWidget *>("head");
//    Q_ASSERT(mTitleBut);

    ////update button
    mUpdateBut = mMainFrame->findChild<QPushButton *>("but_update");
    Q_ASSERT(mUpdateBut);
    ////wifi
    mWifi = mMainFrame->findChild<QLabel *>("lab_wifi");
    Q_ASSERT(mWifi);
	
    mGetPwd = mMainFrame->findChild<QPushButton *>("but_getpwd");

    mGetPwd->hide();

    Q_ASSERT(mGetPwd);
    GENIE2_TRANSLATE_BUTTON(mGetPwd, L_UI_TITLE_GETPWD);

    /////update button hide start////////
    mUpdateBut->setVisible(false);

    ///////////
    mLanSel = mMainFrame->findChild<QLabel *>("text_lanselect");
    Q_ASSERT(mLanSel);
    GENIE2_TRANSLATE_LABEL(mLanSel, L_UI_LAN_SELECT);
#ifdef Q_OS_MACX
    const char *cHideButtons[]={
      "close","min","about"
    };
    for(int i=0;i<sizeof(cHideButtons)/sizeof(cHideButtons[0]);i++)
    {
        QPushButton *but=mMainFrame->findChild<QPushButton *>(cHideButtons[i]);
        if(but)
        {
            but->hide();
        }
    }
    mTitleBut->installEventFilter(&sMouseArrowcontroller);
   // mGo->installEventFilter(&sMouseArrowcontroller);
    mAdvW->installEventFilter(&sMouseArrowcontroller);
    for(int i=0;i<(sizeof(mBbts)/sizeof(mBbts[0]));i++)
    {
        mBbts[i]->installEventFilter(&sMouseArrowcontroller);
    }
   // mTitleBut->setStyle(new QWindowsStyle());
    mStackWidget->setFixedSize(666,420);
#endif


}

void QGenieUiObject::showMainFrame()
{
    Q_ASSERT(mMainFrame);
    mMainFrame->showNormal();
    mMainFrame->activateWindow();
#ifdef Q_OS_WIN32
HWND hwnd=(HWND)mMainFrame->winId();
if(hwnd)
{

  //
   SetWindowPos(hwnd,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
   SetWindowPos(hwnd,HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            ShowWindow(hwnd,SW_RESTORE);
            ShowWindow(hwnd,SW_SHOWNORMAL);

    SetForegroundWindow(hwnd);
}
else
{
   qDebug()<<"error when SetForegroundWindow(hwnd)";
}
#endif
}

void QGenieUiObject::changeLanguage()
{
    QMap<short,_tLinkData *>::Iterator i=mLinkData.begin();
    while(i!=mLinkData.end())
    {
        _tLinkData *t=i.value();
        Q_ASSERT(t);

        if(t->leftBut)
        {
            QString text=(t->uiPara[_tLinkData::UP_LEFT_LANGUAGE].type()==QVariant::Int)?
                        GENIE2_GET_TEXT2(t->uiPara[_tLinkData::UP_LEFT_LANGUAGE].toInt(),t->uuid_idx)
                      :t->uiPara[_tLinkData::UP_LEFT_LANGUAGE].toString();
            t->leftBut->setText(text);
//            t->leftBut->setVisible(!text.isEmpty());
        }
        if(t->bbt)
        {
            t->bbt->updateUi();
        }

        i++;
    }


    //////Change Titlt Update Button Text
    mUpdateBut->setText(mTitleUpdate->getTitleText());
	mUpdateBut->setToolTip(mTitleUpdate->getTitleText());
    mHomeBut->setText(GENIE2_GET_TEXT(L_UI_LEFTBT_HOME));
    QPushButton *but_appstore=mLeftPan->findChild<QPushButton *>("but_appstore");
    if(but_appstore)
    {
        but_appstore->setText(GENIE2_GET_TEXT(L_UI_APPSTORE));
    }

    //////change Wifi label text
    setWifiName(mWifiType);
}

void QGenieUiObject::initlanguageSetting()
{
    GENIE2_LOG("QGenieUiObject::initlanguageSetting()");
    int lanidx=GENIE2_GET_SETTING(LANGUAGE_SETTING).toInt();
    if(lanidx <0)
    {
        QLocale::Language lan=QLocale::system().language();
        struct _lanmap{QLocale::Language s;int idx;};
        _lanmap lanmap[]={
            {QLocale::English,ELT_ENGLISH},
            {QLocale::German,ELT_GERMANY},
            {QLocale::Swedish,ELT_SWEDISH},
            {QLocale::French,ELT_FRENCH},
            {QLocale::Dutch,ELT_DUTCH},
            {QLocale::Italian,ELT_ITALIAN},
            {QLocale::Spanish,ELT_SPANISH},
            {QLocale::Danish,ELT_DANISH},
            {QLocale::Finnish,ELT_FINNISH},
            {QLocale::Norwegian,ELT_NORWEGIAN},
            {QLocale::Greek,ELT_GREEK},
            {QLocale::Portuguese,ELT_PORTUGUESE},
            {QLocale::Russian,ELT_RUSSIAN},
            {QLocale::Polish,ELT_POLISH},
            {QLocale::Hungarian,ELT_HUNGARIAN},
            {QLocale::Czech,ELT_CZECH},
            {QLocale::Slovenian,ELT_SLOVENIAN},
            {QLocale::Slovak,ELT_SLOVAK},
            {QLocale::Romanian,ELT_ROMANIAN},
            {QLocale::Chinese,ELT_CHINESE},
            {QLocale::Korean,ELT_KOREAN},
            {QLocale::Japanese,ELT_JAPANESE},
            {QLocale::Arabic,ELT_ARABIC},
            {QLocale::Turkish,ELT_TURKISH}
        };

        for(int i=0;i<(sizeof(lanmap)/sizeof(lanmap[0]));i++)
        {
            if(lanmap[i].s==lan)
            {
                lanidx=lanmap[i].idx;
                break;
            }
        }
    }
    lanidx=(lanidx<0)?0:lanidx;
    Q_ASSERT(mLanComb);
    if(mLanComb->currentIndex()!=lanidx)
    {
        mLanComb->setCurrentIndex(lanidx);
    }
    else
    {
        slot_LanChanged(lanidx);
    }
}

QVariant QGenieUiObject::parseUiCmd(const QVariant &para_list)
{
    //    qDebug()<<para_list;
    QWidget *w;
    QList<QVariant> list=para_list.toList();
    if(list.size()<4 || list[0].type()!=QVariant::Int ||list[1].type()!=QVariant::Int
       ||list[3].type()!=QVariant::Int)
    {
        return QVariant();
    }
    //cmd,bbtidx,para,uuid
    int cmd=list[0].toInt();
    int uuid=list[3].toInt();
    int bbtidx=list[1].toInt();
    QVariant &para=list[2];

   // qDebug()<<mLinkData2.contains(uuid)<<(mLinkData2[uuid].size());
    if(cmd <0 ||cmd >=UICMD_NUM)
    {
        return QVariant();
    }
    _tLinkData *t=(!mLinkData2.contains(uuid)||bbtidx <0 ||bbtidx >=mLinkData2[uuid].count())?
                NULL:mLinkData2[uuid][bbtidx];





    switch(cmd)
    {
#define _BUTLINE_UI(x,y,z) do{\
    if(t){\
    t->uiPara[_tLinkData::UP_BUTLINE_MODE]=(x);\
    t->uiPara[_tLinkData::UP_BUTLINE_PARA1]=(y);\
    t->uiPara[_tLinkData::UP_BUTLINE_PARA2]=(z);}\
    }while(0)
        case UICMD_BIGBUTTION_BUTLINE_ERROR:
            _BUTLINE_UI(2,-L_UI_STATE,(para.isValid())?para:-L_UI_NOTENABLED);
            break;
        case UICMD_BIGBUTTION_BUTLINE_GOOD:
            _BUTLINE_UI(1,-L_UI_STATE,(para.isValid())?para:-L_UI_BIGBUT_GOOD);
            break;
        case UICMD_BIGBUTTION_BUTLINE_OVERFLOW:
            _BUTLINE_UI(4,-L_UI_STATE,(para.isValid())?para:-L_UI_OVERFLOW);
            break;
        case UICMD_BIGBUTTON_BUTLINE_CLICKHERE:
            _BUTLINE_UI(0,(para.isValid())?para:-L_UI_CLICK_HERE,0);
            break;
        case UICMD_BIGBUTTION_BUTLINE_FIXIT:
            _BUTLINE_UI(3,-L_UI_STATE,-L_UI_CLICKHERE_TOFIX);
            break;
#undef _BUTLINE_UI
        case UICMD_BIGBUTTON_ICON:
            if(t)
            {
                t->uiPara[_tLinkData::UP_ICO]=para;
            }
            break;
        case UICMD_BIGBUTTON_LANGUAGE:
            if(t)
            {
                t->uiPara[_tLinkData::UP_BBT_LANGUAGE]=para;
            }
            break;
        case UICMD_LEFTBUTTON_LANGUAGE:
            if(t)
            {
                t->uiPara[_tLinkData::UP_LEFT_LANGUAGE]=para;
            }
            break;
        case UICMD_SET_FRAMELESS_STYLE:
            w=static_cast<QWidget *>(para.value<void *>());
            _setFramelessStyle(w);
            enableMove(w);
            w->show();
            break;
        case UICMD_CHECK_LEFTBUT:
            checkOtherPlugin(para);

            break;
        case UICMD_TITLE_UPDATE:
            dealUpdateCmd(para);
            break;
        case UICMD_TITLE_SSID:
            setWifiName(para.toString());
            break;
        case UICMD_DIALOG_MODAL:
            return _showModalDialog(para);
        case UICMD_DIALOG_WARRING:
            return _showModalDialog(para,true);
        case UICMD_TRAY_MESSAGE:
            showTrayMessage(UICMD_TRAY_MESSAGE, para);
            break;
        case UICMD_TRAY_TOOLTIP:
            showTrayMessage(UICMD_TRAY_TOOLTIP, para);
            break;
        case UICMD_SHOW_MAINFRAME:
            GENIE2_LOG("popup UICMD_SHOW_MAINFRAME");
            showMainFrame();
            break;
        case UICMD_UNLOCK_SMARTNETWORK:
            unlockSM4Title();
            unlockSM4Splash();
            break;
#ifdef USE_DIRTY_FOCUS_CLEARER
        case UICMD_CLEAR_DIRTY_FOCUSFRAME_FOR_MACOS:
			mDirtyFocusClearer.startRefresh();
            break;
#endif
        default:
            break;
    }


    if(t && t->bbt)
    {
        quint32 flag=0;
        const struct _{
            int cmd;
            quint32 flag;
        } cFlagPara[]={
            {UICMD_BIGBUTTON_ICON,QGenieButtonBig::UF_ICO}
            ,{UICMD_BIGBUTTON_LANGUAGE,QGenieButtonBig::UF_TEXT}
            ,{UICMD_BIGBUTTON_BUTLINE_CLICKHERE,QGenieButtonBig::UF_BUTLINE}
            ,{UICMD_BIGBUTTION_BUTLINE_ERROR,QGenieButtonBig::UF_BUTLINE}
            ,{UICMD_BIGBUTTION_BUTLINE_GOOD,QGenieButtonBig::UF_BUTLINE}
            ,{UICMD_BIGBUTTION_BUTLINE_OVERFLOW,QGenieButtonBig::UF_BUTLINE}
            ,{UICMD_BIGBUTTION_BUTLINE_FIXIT,QGenieButtonBig::UF_BUTLINE}
        };
        for(int i=0;i<sizeof(cFlagPara)/sizeof(cFlagPara[0]);i++)
        {
            if(cFlagPara[i].cmd==cmd)
            {
                flag |=cFlagPara[i].flag;
                break;
            }
        }
        t->bbt->updateUi(flag);
    }
    if(t && t->leftBut && cmd==UICMD_LEFTBUTTON_LANGUAGE)
    {
        QString text=GENIE2_VARIANT_TO_STRING(list[2],t->uuid_idx);
        t->leftBut->setText(text);
        t->leftBut->setVisible(!text.isEmpty());
    }
    return QVariant();

}

//void QGenieUiObject::_createTrayIco()
//{

//}

void QGenieUiObject::checkOtherPlugin(const QVariant &para)
{
    int uuid_idx;
    int bbt_idx=0;
    if(para.type()==QVariant::String)
    {
        uuid_idx=GENIE2_HANDLER_CMD(HCMD_UUID_IDX,para.toString()).toInt();
    }
    else if(para.type()==QVariant::Int)
    {
        uuid_idx=para.toInt();
    }
    else if(para.type()==QVariant::List)
    {
        QVariantList lst=para.toList();
        int c=lst.count();
        Q_ASSERT(c>0);
        uuid_idx=(lst[0].type()==QVariant::Int)?(lst[0].toInt()):GENIE2_HANDLER_CMD(HCMD_UUID_IDX,lst[0]).toInt();
        bbt_idx=(c>1)?(lst[1].toInt()):0;
    }
    //    QList<QVariant> list=para.toList();

    //    if(list.count()<2 ||(list[1].type()!=QVariant::Int))
    //    {
    //        qDebug()<<"QGenieUiObject::checkOtherPlugin error,parameter wrong!!"<<para;
    //        return;
    //    }
    //    bool btemp=true;
    //    int idx=list[1].toInt();
    //    int uuid=(list[0].type()==QVariant::Int)?list[0].toInt()
    //                                           :GENIE2_HANDLER_CMD(HCMD_UUID_IDX,list[0]).toInt(&btemp);
    //    //    if(!btemp || !mLinkData2.contains(uuid) || mLinkData2[uuid].count()<=idx)
    //    //    {
    //    //        qDebug()<<"QGenieUiObject::checkOtherPlugin error,no plugin!!"<<para;
    //    //        return;
    //    //    }

    _tLinkData *t=(mLinkData2.contains(uuid_idx)&&mLinkData2[uuid_idx].count()>bbt_idx)?
                mLinkData2[uuid_idx][bbt_idx]:NULL;
    checkBBt(t);

}

#ifdef USE_DBG_STACK
void QGenieUiObject::registerDBGUi(const QVariant &paras)
{
    tryInitDbgPan();
    QWidget *w=static_cast<QWidget *>(paras.value<void *>());
    Q_ASSERT(w);
    mDbgStack->addWidget(w);
}

void QGenieUiObject::tryInitDbgPan()
{
    if(!mDbgStack)
    {
        mDbgStack=new QStackedWidget;
        mDbgStack->installEventFilter(this);

        qDebug()<<"QGenieUiObject::_doDBG()";
        QUiLoader loader;
        QFile dlg_file(GENIE2_RES("ui/pluginshowdialog.ui"));
        bool btemp=dlg_file.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *dlg=qobject_cast<QWidget *>(loader.load(&dlg_file, NULL));
        Q_ASSERT(dlg);

        QPushButton *but_qss=dlg->findChild<QPushButton *>("but_qss");
        Q_ASSERT(but_qss);
        connect(but_qss,SIGNAL(clicked()),this,SLOT(slot_ReloadQss()));

        //        _setFramelessStyle(dlg);

        mDbgStack->addWidget(dlg);
    }
}

#endif

void QGenieUiObject::registerUi(const QVariant &paras)
{
    //    qDebug()<<"registerUi();";
    QList<QVariant> list=paras.toList();
    if(list.size()<3)
    {
        return;
    }
    QWidget *w=static_cast<QWidget *>(list[0].value<void *>());
    Q_ASSERT(w);
    QString uipara=list[1].toString();
    int uuid_idx=list[2].toInt();
    Q_ASSERT(mStackWidget);


    //    QWidget *proxy=new QWidget;
    //    QHBoxLayout *proxy_layout=new QHBoxLayout(proxy);
    //    proxy->setAutoFillBackground(true);
    //    proxy_layout->addWidget(w);

    // w->setStyleSheet("background-color:yellow");

    QStringList slist=uipara.split(":");
    int siz=slist.size();
    int itemp;
    bool btemp;
    _tLinkData *pLinkData=new _tLinkData;

    pLinkData->uiPara[_tLinkData::UP_BBT_LANGUAGE]=pLinkData->uiPara[_tLinkData::UP_LEFT_LANGUAGE]=
            (siz>0)?
                (  (((itemp=slist[0].toInt(&btemp))||true)&&btemp)?itemp:QVariant(slist[0]) )

              :0;


    pLinkData->priority=(siz>1)?slist[1].toShort():(3000+uuid_idx);
    pLinkData->uuid_idx=uuid_idx;
    pLinkData->uiPara[_tLinkData::UP_ICO]=(siz>=3)?slist[2]:"";
    pLinkData->stackW=w;
    pLinkData->cmd=(siz>=4)?slist[3]:"";
    for(int i=0;i<10;i++)
    {
        if(!mLinkData.contains(pLinkData->priority))
        {
            break;
        }
        pLinkData->priority++;
    }
    Q_ASSERT(!mLinkData.contains(pLinkData->priority));
    mLinkData.insert(pLinkData->priority,pLinkData);
    //    int index;
    if(mStackWidget->indexOf(w) <0)
    {
        mStackWidget->addWidget(w);
    }

}

void QGenieUiObject::deployUi()
{
    GENIE2_LOG("QGenieUiObject::deployUi()");
    QMap<short,_tLinkData *>::Iterator i=mLinkData.begin();
    Q_ASSERT(mLeftPan);
    int bbtidx=0;
    QVBoxLayout*layout=new QVBoxLayout(mLeftPan);

#ifdef Q_OS_MAC
        layout->setSpacing(18);
        layout->setContentsMargins(12, 12, 8, 12);
#else
      	int left = 0, right = 0, top = 0, bottom = 0;
		layout->getContentsMargins(&left, &top, &right, &bottom);
		layout->setContentsMargins(0, top + 60, 0, 0);
		layout->setSpacing(0);
#endif


    Q_ASSERT(layout);
    Q_ASSERT(!mHomeBut);
    mHomeBut=new QPushButton();
	mHomeBut->setObjectName("home_btn");
	mHomeBut->setIconSize(QSize(30, 30));
	mHomeBut->setIcon(QIcon(":/BIGIT/bigit/left_pan_home_icon.png"));
//	mHomeBut->setText("home");
    mHomeBut->setFocusPolicy(Qt::NoFocus);
    mHomeBut->setCheckable(true);
    mHomeBut->setChecked(true);

    mLeftButGroup.addButton(mHomeBut);
    connect(mHomeBut,SIGNAL(clicked()),this,SLOT(slot_LinkButClicked()));
    layout->addWidget(mHomeBut);

    while(i!=mLinkData.end())
    {
        _tLinkData *t=i.value();
        //construct linkdata2
        if(!mLinkData2.contains(t->uuid_idx))
        {
            mLinkData2.insert(t->uuid_idx,QList<_tLinkData *>());
        }
        mLinkData2[t->uuid_idx].append(t);

        //construct linkdata2 end


        QPushButton *leftbut=new QPushButton;
        t->leftBut=leftbut;
        mLeftButGroup.addButton(leftbut);
        leftbut->setCheckable(true);
        leftbut->setUserData(0,(QObjectUserData *)t);
        connect(leftbut,SIGNAL(clicked()),this,SLOT(slot_LinkButClicked()));

        leftbut->setFocusPolicy(Qt::NoFocus);
        layout->addWidget(leftbut);
        if(bbtidx<sizeof(mBbts)/sizeof(mBbts[0]))
        {
            mBbts[bbtidx]->setUserData(0,(QObjectUserData *)t);
            t->bbt=mBbts[bbtidx];
        }


        i++;
        bbtidx++;
    }
    QSpacerItem *spacer=new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(spacer);

    QPushButton *but_appstore=new QPushButton;
    but_appstore->setObjectName("but_appstore");
    but_appstore->setFocusPolicy(Qt::NoFocus);
    connect(but_appstore,SIGNAL(clicked()),this,SLOT(slot_ButClicked()));
    layout->addWidget(but_appstore);

    bool bSMUnlocked=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_UNLOCKED).toBool();
   // but_appstore->setVisible(bSMUnlocked);
	 but_appstore->setVisible(false);
   /* if(bSMUnlocked)
    {
        unlockSM4Title(but_appstore);
    }*///kai.yan modify for genie+

    //    _createTrayIco();


    initlanguageSetting();
#ifdef DBG_MODE
//    mAdvStateMachine->setRouter("abcd");
#endif

}

const char cShowupCode[]="genieshowbnm";

bool QGenieUiObject::eventFilter(QObject *obj, QEvent *env)
{
    QEvent::Type type=env->type();
    QWidget *w=qobject_cast<QWidget *>(obj);
    //if(w==mAdvW && w  )
    //{
    //    if(QEvent::MouseButtonRelease==type)
    //    {
    //        qDebug()<<"QGenieAdvWidget::mouseReleaseEvent";
    //        QMouseEvent *mouseenv=static_cast<QMouseEvent *>(env);
    //        if (mouseenv && mouseenv->button () ==Qt::LeftButton )
    //        {
    //            if(mAdvW->geometry().contains(mouseenv->pos()))
    //            {
    //                QUrl qurl(mCurLink);
    //                QDesktopServices::openUrl(qurl);
    //                GENIE2_STATISTICS("H11");
    //            }
    //        }
    //    }
    //    else if(QEvent::Enter==type)
    //    {
    //        mAdvW->setCursor(Qt::PointingHandCursor);
    //    }
    //    else if(QEvent::Leave==type)
    //    {
    //        mAdvW->setCursor(Qt::ArrowCursor);
    //    }
    //}
    if(w==mMainFrame && type==QEvent::KeyPress)
    {
        QKeyEvent *keyenv=static_cast<QKeyEvent *>(env);
        if(keyenv && !(keyenv->text().isEmpty()))
        {
            static int cheetidx=0;
            QChar c=keyenv->text()[0];
            cheetidx=(c!=cShowupCode[cheetidx])?
                        ((c==cShowupCode[0])?1:0)
                      :(cheetidx+1);
            if(cheetidx !=0)
            {
                qDebug()<<"debug"<<c;
            }
            if(cheetidx>=(sizeof(cShowupCode)/sizeof(cShowupCode[0])-1))
            {
                _doDBG();
            }
        }

    }
    if( (QEvent::MouseButtonPress==type
         ||QEvent::MouseButtonRelease==type
         ||QEvent::MouseMove==type)
        && (w)
        && mMoveWidgetData.contains(w))
    {
        QMouseEvent *mouseenv=static_cast<QMouseEvent *>(env);
        switch(type)
        {
            case QEvent::MouseButtonPress:
                if(mouseenv->button() &Qt::LeftButton)
                {
                    mMoveWidgetData[w]->mDrag=true;
                    mMoveWidgetData[w]->mDragPoint = mouseenv->globalPos() - w->frameGeometry().topLeft();
                    //                return true;
                }
                break;
            case QEvent::MouseButtonRelease:
                if(mouseenv->button() & Qt::LeftButton)
                {
                    mMoveWidgetData[w]->mDrag=false;
                    //                return true;
                }
                break;
            case QEvent::MouseMove:
                if ((mouseenv->buttons() & Qt::LeftButton) && mMoveWidgetData[w]->mDrag && !w->isFullScreen())
                {
                    w->move(mouseenv->globalPos() - mMoveWidgetData[w]->mDragPoint);

                    //                return true;
                }
                break;
        }
    }
#ifdef USE_DBG_STACK
    //    if(w==mDbgStack && QEvent::MouseButtonPress==type)
    //    {
    //        qDebug()<<"dbg press";
    //        mSavedData=0;
    //    }
    if(w==mDbgStack  )
    {
        if(QEvent::Move==type)
        {
            static QDateTime sDT=QDateTime::currentDateTime().addDays(-1);
            QDateTime curDT=QDateTime::currentDateTime();

            if(sDT.addMSecs(500)<curDT)
            {
                mSavedData=0;
            }
            sDT=curDT;
            //        qDebug()<<"dbg widget move";
            QMoveEvent *move_env=static_cast<QMoveEvent *>(env);

            if(move_env->pos().x()==move_env->oldPos().x()
                ||move_env->pos().y()==move_env->oldPos().y())
                {
                return false;
            }
            quint8 direction=0|((move_env->pos().x()>move_env->oldPos().x())?(1<<1):0)
                             |((move_env->pos().y()>move_env->oldPos().y())?(1<<0):0);
            quint8 lastpos_offset=(mSavedData&(3<<8))>>8;
            quint8 lastdirection=(mSavedData & (3<<(lastpos_offset<<1)))>>(lastpos_offset<<1);

            if(direction != lastdirection)
            {
                //            qDebug()<<direction<<lastdirection;
                const quint8 curpos_offset=(3&(lastpos_offset+1));
                //            qDebug()<<"direction change!";
                //update cur pos;
                mSavedData=(mSavedData& ~(3<<8))+ (curpos_offset<<8 );
                //save direction data
                mSavedData=(mSavedData& ~(3<<(curpos_offset<<1)) )+(direction<<(curpos_offset<<1));

                const quint8 direction_queue=mSavedData&0xff;
                //            QString stemp;
                //            stemp.sprintf("0x%x",direction_queue);
                //            qDebug()<<stemp;
                const quint8 cPageUpPara[]={0x78,0xe1,0x87,0x1e};
                for(int i=0;i<sizeof(cPageUpPara)/sizeof(cPageUpPara[0]);i++)
                {
                    if(cPageUpPara[i]==direction_queue)
                    {
                        mSavedData=mSavedData & ~0xff;
                        int c=mDbgStack->count();
                        mDbgStack->setCurrentIndex((mDbgStack->currentIndex()+c+1)%c);
                        qDebug()<<"page up"<<c;
                    }
                }
                const quint8 cPageDownPara[]={0x2d,0xb4,0xd2,0x4b};
                for(int i=0;i<sizeof(cPageDownPara)/sizeof(cPageDownPara[0]);i++)
                {
                    if(cPageDownPara[i]==direction_queue)
                    {
                        mSavedData=mSavedData & ~0xff;
                        qDebug()<<"page down";
                        int c=mDbgStack->count();
                        mDbgStack->setCurrentIndex((mDbgStack->currentIndex()+c-1)%c);

                    }
                }

            }
        }
        else if(QEvent::KeyRelease ==type)
        {
            QKeyEvent *keyenv=static_cast<QKeyEvent *>(env);
            if(keyenv->key()==Qt::Key_Left)
            {
                int c=mDbgStack->count();
                mDbgStack->setCurrentIndex((mDbgStack->currentIndex()+c+1)%c);
                qDebug()<<"page up"<<c;

            }
            else if(keyenv->key()==Qt::Key_Right)
            {
                qDebug()<<"page down";
                int c=mDbgStack->count();
                mDbgStack->setCurrentIndex((mDbgStack->currentIndex()+c-1)%c);

            }
        }
    }
	else if(QEvent::ToolTip == type)
	{
		mUpdateBut->setToolTip(mTitleUpdate->getTitleText());
	}
#endif
    return false;

}

void QGenieUiObject::slot_MovableWidgetDestroyed(QObject *obj)
{   
    QWidget *w=qobject_cast<QWidget *>(obj);
    Q_ASSERT(w);
    _tMoveData *t=mMoveWidgetData[w];
    delete t;
    mMoveWidgetData.remove(w);
}

void QGenieUiObject::slot_LinkButClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    if(but)
    {
        bool bNewlyChecked=false;
        if(but==mHomeBut )
        {
            bNewlyChecked=checkBBt(NULL);
        }
        else
        {
            _tLinkData *t=(_tLinkData *)but->userData(0);
            bNewlyChecked=checkBBt(t);
         }
        //statistics function
        if(bNewlyChecked)
        {
            bool isBbt=false;
            for(int i=0;i<(sizeof(mBbts)/sizeof(mBbts[0]));i++)
            {
                if(but==mBbts[i])
                {
                    isBbt=true;
                    break;
                }
            }
            GENIE2_STATISTICS(isBbt?"H3":"H2");

        }
    }

}

bool QGenieUiObject::checkBBt(_tLinkData *t)
{
    bool ret=false;
    if(t && (t!=mCurrentLinkData) )
    {
        if(mCurrentLinkData)
        {
            GENIE2_TELL_OTHERPLUGIN(mCurrentLinkData->uuid_idx,I2CMD_UI_LOWER
                                    ,mCurrentLinkData->cmd);
        }
        mCurrentLinkData=t;
        ret=(mStackWidget->currentWidget()!=t->stackW);
        mStackWidget->setCurrentWidget(t->stackW);

        //tell the plugin click message
        GENIE2_TELL_OTHERPLUGIN(t->uuid_idx,I2CMD_UI_ACTION,t->cmd);
        t->leftBut->setChecked(true);
#ifdef USE_DIRTY_FOCUS_CLEARER
        mDirtyFocusClearer.startRefresh();
#endif		
    }
    else if(!t)
    {
        mHomeBut->setChecked(true);
        if(mCurrentLinkData)
        {
            GENIE2_TELL_OTHERPLUGIN(mCurrentLinkData->uuid_idx,I2CMD_UI_LOWER
                                    ,mCurrentLinkData->cmd);
        }
        ret=(mStackWidget->currentWidget()!=mHomePage);
        mStackWidget->setCurrentWidget(mHomePage);
        mCurrentLinkData=NULL;
    }
    return ret;
}

void QGenieUiObject::slot_LanChanged(int i)
{
    GENIE2_LOG("QGenieUiObject::slot_LanChanged");
    const char * cTypeStr[ELT_NUM]={
        "enu",    "deu",    "sve",    "fra",    "nld",    "ita",    "esp",
        "dan",    "fin",    "nor",    "ell",    "ptb",    "rus",    "plk",
        "hun",    "csy",    "slv",    "sky",    "rom",    "chs",    "kor",
        "jpn",    "ara"};

    if(0<=i&&i<sizeof(cTypeStr)/sizeof(cTypeStr[0]))
    {
        QString font_name="Arial";
        QString fontsetting_fname=GENIE2_RES("fontsetting")+"/fontsetting_"+cTypeStr[i]+".txt";
        if(QFile::exists(fontsetting_fname))
        {
            QFile f(fontsetting_fname);
            if(f.open(QIODevice::ReadOnly))
            {
                QTextStream s(&f);
                s>>font_name;
            }
        }

        GENIE2_HANDLER_CMD(HCMD_LANGUAGE_CHANGE,int(i));
        GENIE2_NOTIFY_MESSAGE(SCCMD_LANGUAGE,cTypeStr[i]);
//        mAdvStateMachine->setLan(cTypeStr[i]);
        changeLanguage();
        _translateIco();

     //   qApp->setFont(font);
     //   QWidgetList wlist=qApp->allWidgets();
     //   foreach(QWidget *w,wlist)
     //   {
     //       w->setFont(font);
      //      w->update();
      //  }
        QString stylesheet=QString("*{ font-family: \"%1\"}").arg(font_name);
        qApp->setStyleSheet(stylesheet);


        ////set current language
        mCurLan = cTypeStr[i];
    }


}



void QGenieUiObject::slot_ChangeAdvPic(QString filepath, QString link)
{
    qDebug()<<"QGenieAdvWidget::slot_ChangeAdvPic"<<filepath<<link;
	QFileInfo fileInfo(filepath);
	QString stylesheet;
	if(fileInfo.exists())
	{
		 stylesheet = QString("background-image: url(%1);").arg(filepath);
	}else
	{
		stylesheet = QString("background-image: url(%1);").arg(GENIE2_RES("images/fg_ad_frame.png"));
	}
   
	stylesheet.replace("\\","/");
    mCurLink=link;
    //if(mAdvW)
    //{
    //    mAdvW->setStyleSheet(stylesheet);
    //}
}

void QGenieUiObject::slot_StartSearch()
{
 //   if(mUrlTimer.isActive())
 //   {
 //       return;
 //   }
 //   mUrlTimer.start(1000);
	//QString txturl = mSearchEdt->text().trimmed();
	/////---kai.yan add for search error
	//QRegExp reg("[/%|/&|/*|/<|/>|:]");
	//txturl = txturl.replace(reg,"");
	//////--kai.yan end 
 //   if(txturl!="")
 //   {
 //       QUrl url("http://support.netgear.com/search/"+txturl);
 //       QDesktopServices::openUrl(url);
 //       GENIE2_STATISTICS("H5");
 //       GENIE2_STATISTICS_SEARCH(txturl);
 //   }

}

void QGenieUiObject::_showAboutDialog()
{




    QUiLoader loader;
    QFile dlg_file(GENIE2_RES("ui/aboutdialog.ui"));
    bool btemp=dlg_file.open(QFile::ReadOnly);
    Q_ASSERT(btemp);
    QWidget *w=loader.load(&dlg_file, mMainFrame);

    QDialog *dlg=static_cast<QDialog *>(w);
    _setFramelessStyle(dlg);

    QGenieWaterWindow *waterW=new QGenieWaterWindow(dlg);
//    waterW->setBG(GENIE2_RES("images/bg_about.png"));
////    waterW->setWaterMark(strMark);
//    waterW->setTopPadding(73);
    waterW->move(8,40);

    QLabel *labTitle = w->findChild<QLabel *>("label_title");
    Q_ASSERT(labTitle);
    labTitle->setText(GENIE2_GET_TEXT(L_UI_TITLE_ABOUT));
    QPushButton *closebut=w->findChild<QPushButton *>("close");
    if(closebut)
    {
        GENIE2_TRANSLATE_BUTTON(closebut,L_UI_CLOSE);
    }

    dlg->exec();
    GENIE2_SAFEDELETE(dlg);
}

void QGenieUiObject::slot_ButClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(but);
    QString objname=but->objectName();
    if(objname=="go")
    {
        slot_StartSearch();

    }
    else if(objname=="about")
    {
        _showAboutDialog();

    }
    else if(objname == "close")
    {
        Q_ASSERT(mMainFrame);
        mMainFrame->showMinimized();
#ifndef PLATFORM_MACOSX
        mMainFrame->hide();
#endif
    }
    else if(objname == "but_update")
    {
        mTitleUpdate->butClicked();

    }
    else if(objname == "min")
    {
        ////最小化
        Q_ASSERT(mMainFrame);
        mMainFrame->showMinimized();
    }
    else if(objname == "but_getpwd")
    {
        ////取得无线密码

        GENIE2_STATISTICS("H1");
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_NETCONFIG, EPCC_TURN_TO_WIRELESS_PWD, 0);
        GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,PLUGIN_UUID_NETCONFIG);
    }
    else if(objname == "head")
    {
        //title
        if(mUrlTimer.isActive() )
        {
            return;
        }
        mUrlTimer.start(1000);//=QTime::currentTime();
        //QString strUrl=GENIE2_GET_SETTING(TITLE_URL).toString();
        //QDesktopServices::openUrl(strUrl); ///kai.yan remove
    }
    else if(objname=="but_appstore")
    {
        if(mUrlTimer.isActive())
        {
            return;
        }
        mUrlTimer.start(1000);
        QString strUrl=GENIE2_GET_SETTING(APPSTORE_LINK).toString();
        QDesktopServices::openUrl(strUrl);
    }
}

void QGenieUiObject::slot_SaveSplash()
{
    QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();
//    mAdvStateMachine->setRouter(model);
    QString oldmod=GENIE2_GET_SETTING(PLUGIN_ROUTER_MODEL).toString();
    qDebug()<<"---slot_SaveSplash:model--"<<model<<"oldmod:"<<oldmod;
    if((oldmod !=model)&&!model.isEmpty())
    {
        GENIE2_SET_SETTING(PLUGIN_ROUTER_MODEL,model);
        QImageNameGengerator gener(model);
        QPixmap router_pixmap(gener.getImageName(IE_B31));
        QString fname=QString(NETGEAR_DIR)+"splash.png";

//        if(QFile::exists(fname))
//        {

//        }
        bool bSMUnlocked=GENIE2_GET_SETTING(PLUGIN_SMARTNETWORK_UNLOCKED).toBool();
		bSMUnlocked = false;///kai.yan add for genie+

        QImage splash_img(bSMUnlocked?GENIE2_RES("images/splash_gadd.png"):":/splash");
        QPainter p(&splash_img);
        int x=((splash_img.width()-router_pixmap.width())>>1);
        p.drawPixmap(x,55,router_pixmap);
        splash_img.save(fname);
    }
}

#ifdef GENIE2_TRAY
void QGenieUiObject::_translateIco()
{
    QString mode="obj_open[action]=\"%1\",obj_upgrade[action]=\"%2\",obj_settings[menu]=\"%3\""
            ",obj_netchange[action]=\"%5\",obj_autostart[action]=\"%6\""
            ",obj_exit[action]=\"%7\"";
    mode=mode.arg(GENIE2_GET_TEXT(L_UI_RESTORE),GENIE2_GET_TEXT(L_UI_UPDATE),GENIE2_GET_TEXT(L_UI_TRAY_SETTING)
                  ,GENIE2_GET_TEXT(L_UI_TRAY_NETCONNECTION_CHANGE),GENIE2_GET_TEXT(L_UI_TRAY_AUTOSTART)
                  ,GENIE2_GET_TEXT(L_UI_EXIT));

    mTrayManager->uiCommand(IGenieTrayIco::TUIC_TRANSLATE_ACTION,mode);

    int netstate=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_CONNECTION_STATE).toInt();
    QString stateString=(netstate==1)?GENIE2_GET_TEXT(L_UI_TRAY_TOOLTIP_DISCONNECT)
                                    :GENIE2_GET_TEXT(L_UI_TRAY_TOOLTIP_CONNECT);
    QString ico=(netstate==1)?"images/ico_tray_error.ico":"images/ico_tray_good.ico";
    //mMainFrame->setWindowIcon(QIcon(ico));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SHOW_ICO,GENIE2_RES(ico));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SET_TOOLTIP,stateString);

}

void QGenieUiObject::slot_trayActionTrigged(const QString &objname)
{
    QStringList lst=objname.split(",");
    if(lst.count()!=2)
    {
        QMessageBox::warning(NULL,"slot_trayActionTrigged wrong",objname);
        return;
    }

    if(lst[0]=="obj_open" || lst[0] == "doubleClick"
       || lst[0]=="trigged")
    {
        GENIE2_LOG("popup tray");
        showMainFrame();

    }
    else if(lst[0]=="obj_exit")
    {

        QList<QDialog *> dlist=mMainFrame->findChildren<QDialog *>();
        int c=dlist.count();
        for(int i=0;i<c;i++)
        {
            dlist[i]->reject();
        }


        //        mTrayManager->uiCommand(IGenieTrayIco::TUIC_HIDE_ICO);
        QTimer::singleShot(0,this,SLOT(slot_Exit()));




    }
    else if(lst[0]=="obj_autostart")
    {
#ifdef Q_OS_WIN32
        QSettings setting_lm("HKEY_LOCAL_MACHINE\\Software\\NETGEARGenie",QSettings::NativeFormat);
        if(!setting_lm.contains("Install_Dir"))
        {
            QString stemp=qApp->applicationDirPath().replace("/","\\")+"\\NETGEARGenie.exe";
            setting_lm.setValue("Install_Dir",stemp);
        }

        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);
        if(lst[1]=="1")
        {
            //QString apppath=qApp->applicationDirPath();
            QString appname=qApp->applicationFilePath();
            QString cmd=QString("\"%1\" -mini -redirect").arg(appname.replace("/","\\"));

            settings.setValue("NETGEARGenie",cmd);
        }
        else{
            settings.remove("NETGEARGenie");
        }
#else
        GENIE2_SET_SETTING(MACOS_AUTOSTART,(lst[1]=="1"));

#endif
        GENIE2_STATISTICS((lst[1]=="1")?"H9":"H8");
    }
    else if(lst[0]=="obj_netchange")
    {
        GENIE2_SET_SETTING(SHOW_NETWORK_CHANGE,(lst[1]=="1"));
        GENIE2_STATISTICS((lst[1]=="1")?"H7":"H6");
    }
    else if(lst[0]=="obj_upgrade")
    {
        //check if plugin loaded
        GENIE2_HANDLER_CMD(HCMD_LOADPLUGIN,PLUGIN_FILE_UPDATE);
        //tell message to plugin
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_UPDATE, EPCC_SOFTWARE_UPDATE, mCurLan);
    }

}

void QGenieUiObject::slot_InitTray()
{
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_CREATE_TRAY,GENIE2_RES("ui/traymenu.xml"));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SHOW_ICO,GENIE2_RES("images/ico_tray_good.ico"));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SET_QSS,GENIE2_RES("ui/traymenu.qss"));
#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);
    bool b1=settings.contains("NETGEARGenie");
#else
    bool b1=GENIE2_GET_SETTING(MACOS_AUTOSTART).toBool();

#endif
    bool b2=GENIE2_GET_SETTING(SHOW_NETWORK_CHANGE).toBool();
    QString sCheckActionCmd="obj_autostart=%1,obj_netchange=%2";
    sCheckActionCmd=sCheckActionCmd.arg((b1?"1":"0"),(b2?"1":"0"));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_ACTION_STATE,sCheckActionCmd);

    _translateIco();
    //    mTrayManager->initUi(b1,b2);
}

void QGenieUiObject::_setFramelessStyle(QWidget *w)
{
    //QMessageBox::warning(NULL,"ddddd","QGenieUiObject::_setFramelessStyle");
    QDialog *dlg=qobject_cast<QDialog *>(w);
#ifdef Q_OS_MACX
    Qt::WindowFlags flag= (dlg)?(Qt::Dialog| Qt::FramelessWindowHint|Qt::Popup):(Qt::FramelessWindowHint);
#else

    Qt::WindowFlags flag= (dlg)?(Qt::Dialog| Qt::FramelessWindowHint):(Qt::FramelessWindowHint);
    if(dlg)
    {
        dlg->setModal(true);
    }
#endif
    w->setWindowFlags(flag);
#ifdef Q_OS_WIN32
    w->setAttribute(Qt::WA_TranslucentBackground);
#endif
    enableMove(w);

    QFile f(GENIE2_RES("ui/framelessframe.qss"));
    bool btemp=f.open(QIODevice::ReadOnly);
    Q_ASSERT(btemp);
    QString stemp=f.readAll();
    w->setStyleSheet(stemp);
}

void QGenieUiObject::_doDBG()
{
#ifdef USE_DBG_STACK

    tryInitDbgPan();
    QTableWidget *tabW=mDbgStack->findChild<QTableWidget *>("tabW");
    Q_ASSERT(tabW);

    QAbstractItemModel *model=tabW->model();
    QVariant v;
    v.setValue((void *)model);
    GENIE2_HANDLER_CMD(HCMD_INIT_DBGMODLE,v);
    mDbgStack->show();
#endif



    /******Chensi 2012-2-20********/
    //    QPushButton *butSta = dlg->findChild<QPushButton *>("but_static");
    //    Q_ASSERT(butSta);
    //    connect(butSta, SIGNAL(clicked()),this,SLOT(slot_but_static()));



    //    dlg->exec();
    //    GENIE2_SAFEDELETE(dlg);
}

#endif //GENIE2_TRAY

bool QGenieUiObject::_showModalDialog(const QVariant &para,bool warring)
{
    QList<QDialog *> dlist=mMainFrame->findChildren<QDialog *>();
    int c=dlist.count();
    for(int i=0;i<c;i++)
    {
        dlist[i]->reject();
    }
    QGenieModalDialog tDialog(mMainFrame);
    QStringList listPara = para.toStringList();
    QString strTitle = listPara.value(0);
    QString strExplain = listPara.value(1);
    tDialog.setTitle(strTitle);
    tDialog.setExplain(strExplain);
    if(warring)
    {
        QPushButton *but_cancel=(&tDialog)->findChild<QPushButton*>("but_cancel");
        Q_ASSERT(but_cancel);
        but_cancel->setVisible(false);
    }
    _setFramelessStyle(&tDialog);
    return tDialog.exec();
}

void QGenieUiObject::slot_Exit()
{
    mMainFrame->showNormal();
    mMainFrame->activateWindow();
    quint32 &flags=QGenieIface2ShareObject::sSingleInstance->mFlags;

    if(!(flags & SOF_EXIT_DIALOG_ON))
    {

        flags |= SOF_EXIT_DIALOG_ON;
        QUiLoader loader;
        QFile dlg_file(GENIE2_RES("ui/exitdialog.ui"));
        bool btemp=dlg_file.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&dlg_file, mMainFrame);

        QDialog *dlg=static_cast<QDialog *>(w);
        _setFramelessStyle(dlg);
        dlg->setWindowFlags(dlg->windowFlags()|Qt::WindowStaysOnTopHint);

        QLabel *labTitle = w->findChild<QLabel *>("label_title");
        Q_ASSERT(labTitle);
        labTitle->setText(GENIE2_GET_TEXT(L_UI_EXIT));
        QLabel *labContent = w->findChild<QLabel *>("label_content");
        Q_ASSERT(labContent);
        labContent->setText(GENIE2_GET_TEXT(L_UI_QUESTION_EXIT));
        QPushButton *okBut=w->findChild<QPushButton *>("ok");
        QPushButton *cancelBut = w->findChild<QPushButton *>("cancel");
        if(okBut && cancelBut)
        {
            GENIE2_TRANSLATE_BUTTON(okBut,L_UI_OK);
            GENIE2_TRANSLATE_BUTTON(cancelBut, L_UI_CANCEL);
        }



        if(1==dlg->exec())
        {
            GENIE2_HANDLER_CMD(HCMD_EXIT,0);
#ifndef COMPLETE_EXIT
            if(!QApplication::arguments().contains("-dbg"))
            {
                _exit(0);
            }
#endif

            mMainFrame->close();
            GENIE2_SAFEDELETE(dlg);
            mTrayManager->uiCommand(IGenieTrayIco::TUIC_HIDE_ICO);

            QList<QPushButton*> leftbuts=mLeftPan->findChildren<QPushButton*>();
            int c=leftbuts.count();
            for(int i=0;i<c;i++)
            {
                leftbuts[i]->setUserData(0,NULL);
            }
            QList<QPushButton*> bbts=mHomePage->findChildren<QPushButton *>();
            c=bbts.count();
            for(int i=0;i<c;i++)
            {
                bbts[i]->setUserData(0,NULL);
            }
//            GENIE2_SAFEDELETE(mAdvStateMachine) ;
            QGenieIface2ShareObject::sSingleInstance->mMainFrame=NULL;
            GENIE2_SAFEDELETE(mStackWidget);
            GENIE2_SAFEDELETE(mMainFrame);

#ifdef USE_DBG_STACK
            GENIE2_SAFEDELETE(mDbgStack);
#endif

            qApp->quit();
            //    GENIE2_SAFEDELETE(mMainFrame);
        }
        flags &=~SOF_EXIT_DIALOG_ON;
        delete dlg;
//        sbInExitDialog=false;
    }
    else
    {
        mMainFrame->showNormal();
    }
}

//=================================
//  static
//=================================

////////////////////////////////////////////////////////////
//        NAME: slot_but_static()
// DESCRIPTION: static button clicked.
//   ARGUMENTS: None.
//     RETURNS: None.
//      AUTHOR: Chensi
//        DATA: 2012-2-20
////////////////////////////////////////////////////////////
void QGenieUiObject::slot_but_static()
{
    GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_STATISTIC, I2CMD_OTHER1, 0);
}

void QGenieUiObject::slot_ReloadQss()
{
    QFile qss_file(GENIE2_RES("ui/QGenieMainUi.qss"));
    if(qss_file.open(QIODevice::ReadOnly))
    {
        mMainFrame->setStyleSheet(qss_file.readAll());
        qss_file.close();
    }
}


//=================================
//  update
//=================================

////////////////////////////////////////////////////////////
//        NAME: slot_updateIcon()
// DESCRIPTION: update title icon.
//   ARGUMENTS: None.
//     RETURNS: int nType.
//      AUTHOR: Chensi
//        DATA: 2012-2-16
////////////////////////////////////////////////////////////
void QGenieUiObject::slot_updateIcon()
{
    QString stemp=mTitleUpdate->getTitleText();
    mUpdateBut->setText(stemp);
    mUpdateBut->setVisible(!stemp.isEmpty());
}


////////////////////////////////////////////////////////////
//        NAME: dealUpdateCmd()
// DESCRIPTION: Change Update Title icon.
//   ARGUMENTS: None.
//     RETURNS: const QVariant &paras.
//      AUTHOR: Chensi
//        DATA: 2012-2-16
////////////////////////////////////////////////////////////
void QGenieUiObject::dealUpdateCmd(const QVariant &paras)
{
    qDebug() << "QGenieUiObject::dealUpdateCmd();";
    mTitleUpdate->command(paras.toString());
}

void QGenieUiObject::unlockSM4Title(QPushButton *but_appstore,QWidget *head )
{
    if(!but_appstore)
    {
        but_appstore=mLeftPan->findChild<QPushButton *>("but_appstore");
    }
    if(!head)
    {
        head=mMainFrame->findChild<QWidget *>("head");
    }
    Q_ASSERT(but_appstore);
    but_appstore->setVisible(false);///kai.yan modify for remove genie+

     Q_ASSERT(head);
    head->setStyleSheet(QString("background-image: url(%1);"
                                "background-repeat: none;")
                        .arg(GENIE2_RES("images/fg_title_frame.png")));




}


void QGenieUiObject::unlockSM4Splash()
{
    GENIE2_SET_SETTING(PLUGIN_ROUTER_MODEL,"");
    QString fname=QString(NETGEAR_DIR)+"splash.png";
    QFile f(fname);
    if(f.exists())
    {
        f.remove();
    }
    QTimer::singleShot(qrand()%3000,this,SLOT(slot_SaveSplash()));

//    QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();
//    QString oldmod=GENIE2_GET_SETTING(PLUGIN_ROUTER_MODEL).toString();

}

//=================================
//  Wifi
//=================================


////////////////////////////////////////////////////////////
//        NAME: setWifiName()
// DESCRIPTION: show Wifi name.
//   ARGUMENTS: const QString &ssid.
//     RETURNS: void.
//      AUTHOR: Chensi
//        DATA: 2012-2-16
////////////////////////////////////////////////////////////
void QGenieUiObject::setWifiName(const QString &ssid)
{
    mWifiType = ssid;

    QString textname = ssid;
    if(!ssid.isEmpty())
    {
        textname = QString(TITLE_WIFI_TEXT_MODE).arg(GENIE2_GET_TEXT(L_UI_WIRELES_NETWORK), ssid);
    }

    mWifi->setText(textname);
    mWifi->setVisible(!ssid.isEmpty());
    qDebug()<<"QGenieUiObject::setWifiName(const QString &ssid):"<<textname;
}

//=================================
//  trayicon
//=================================

////////////////////////////////////////////////////////////
//        NAME: showTrayMessage()
// DESCRIPTION: None.
//   ARGUMENTS: None.
//     RETURNS: void.
//      AUTHOR: Chensi
//        DATA: 2012-3-9
////////////////////////////////////////////////////////////
void QGenieUiObject::showTrayMessage(int cmd, const QVariant &para_list)
{
	QStringList listPara;
	QString strTxt;

	if(mTrayManager)
	{
		listPara = para_list.toStringList();
		if(listPara.count() > 0)
		{
			strTxt = listPara.value(0);
		}
		else
		{
			return;
		}
		switch(cmd)
		{
		case UICMD_TRAY_TOOLTIP:
			mTrayManager->uiCommand(IGenieTrayIco::TUIC_SET_TOOLTIP, strTxt);
			break;
		case UICMD_TRAY_MESSAGE:
			mTrayManager->uiCommand(IGenieTrayIco::TUIC_TRAY_MESSAGE, strTxt);
			break;
		default:
			break;
		}
	}
}



void QGenieUiObject::setNetState(int state)
{
    QString stateString=(state==1)?GENIE2_GET_TEXT(L_UI_TRAY_TOOLTIP_DISCONNECT)
                                    :GENIE2_GET_TEXT(L_UI_TRAY_TOOLTIP_CONNECT);
    QString ico=(state==1)?"images/ico_tray_error.ico":"images/ico_tray_good.ico";
    mMainFrame->setWindowIcon(QIcon(GENIE2_RES(ico)));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SHOW_ICO,GENIE2_RES(ico));
    mTrayManager->uiCommand(IGenieTrayIco::TUIC_SET_TOOLTIP,stateString);
    static int sSavedState=-1;
    if(sSavedState != state && sSavedState >=0)
    {
        int lan_temp=-1;/*(state==1)?L_UI_TRAY_MESSAGE_DISCONNECT
            :((state==0)?L_UI_TRAY_MESSAGE_CONNECT:L_UI_TRAY_TOO_FLOW);*/
        bool show_hightraffic=GENIE2_GET_SETTING(PLUGIN_TRAFFIC_WARRING).toBool();
        bool show_netchange=GENIE2_GET_SETTING(SHOW_NETWORK_CHANGE).toBool();

        if(show_hightraffic &&state==2)
        {
            lan_temp=L_UI_TRAY_TOO_FLOW;
        }
        if(lan_temp <0)
        {
            Q_ASSERT(sSavedState>=0 && sSavedState<=2);
            Q_ASSERT(state>=0 && state<=2);
            const int cPara[]={0,1,0};
            if(cPara[sSavedState]!=cPara[state] && show_netchange)
            {
                lan_temp=cPara[state]?L_UI_TRAY_MESSAGE_DISCONNECT:L_UI_TRAY_MESSAGE_CONNECT;
            }
        }
        if(lan_temp >=0)
        {
            mTrayManager->uiCommand(IGenieTrayIco::TUIC_TRAY_MESSAGE,
                                  GENIE2_GET_TEXT(lan_temp));
        }

    }
    sSavedState=state;

}

























