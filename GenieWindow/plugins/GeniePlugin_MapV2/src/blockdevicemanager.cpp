#include "blockdevicemanager.h"
#include "topology.h"
#include "topologyview.h"
#include "networkmapview.h"
#include "netscanningcontroller.h"
#include "devicedetailviewframe.h"
#include <QUiLoader>


#include "language_Map.h"
#include "GeniePlugin_MapV2.h"

#include "QProgressIndicator.h"

#include <QStackedLayout>
#include <QTimer>

#define LOGIN_WIDGET_STYLEFILE ":/styles/login.css"
#define LOGIN_FALED_UI_PRESENTATION_TIME 5000 //5 seconds

BlockDeviceManager::BlockDeviceManager(NetworkMapView *view, NetScanningController *controlproxy)
    :QObject(view)
    ,mCmdAfterLoginWidgetOff(BLOCKDEVICE_FSM::BDC_NONE)
    ,mControlProxy(&controlproxy->m_scp)
    ,mTopologyView(view->m_topologyView)
    ,mLogedIn(false)
    ,mTopLeftTextIdx(-1)
//    ,mMHandler(NULL)
    ,m_panelLayout(0)
    ,m_oldState(BLOCKDEVICE_FSM::FS_END)
    ,mDeviceCount(view->m_devTotalCount)
    ,m_ctrPanelCanVisible(true)
{
    mTopLeftW=new QWidget();
    mTopLeftW->setMaximumHeight(21);
    view->addBlockDeviceControlPanel(mTopLeftW);

    mCheckBox=new QCheckBox(mTopLeftW);
    mCheckBox->setFocusPolicy(Qt::NoFocus);

    mTopLeftLabelFrame=new QWidget(mTopLeftW);
    QHBoxLayout *layout=new QHBoxLayout(mTopLeftLabelFrame);
    layout->setContentsMargins(0,0,0,0);

    mTopLeftLabel=new QLabel(mTopLeftW);
#ifdef SIMPLE_TOPLEFT_TEXT
    mPi=new QProgressIndicator(mTopLeftLabelFrame);
    //mPi->move(100,0);
    mPi->setFixedSize(20,20);
    mPi->setSizePara(1.0);
#endif
    layout->addWidget(mTopLeftLabel);
    layout->addWidget(mPi);
    QSpacerItem *verticalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(verticalSpacer);

    m_panelLayout = new QStackedLayout(mTopLeftW);
    m_panelLayout->addWidget(mCheckBox);
    m_panelLayout->addWidget(mTopLeftLabelFrame);

    m_panelLayout->setCurrentWidget(mCheckBox);

    mTopLeftW->setVisible(false);
    mCheckBox->setEnabled(false);
    connect(mCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onCheckBoxStateChanged(int)));



    QUiLoader loader;
    QFile file(GENIE2_RES("ui/maplogin.ui"));
    file.open(QFile::ReadOnly);
    mLoginFrame=loader.load(&file, view);
    file.close();
    mLoginFrame->setAttribute(Qt::WA_TranslucentBackground);
    mLoginFrame->installEventFilter(this);
    mLoginFrame->hide();

	QFile f(GENIE2_RES("ui/framelessframe.qss"));
    bool btemp=f.open(QIODevice::ReadOnly);
    Q_ASSERT(btemp);
    QString stemp=f.readAll();
    mLoginFrame->setStyleSheet(stemp);

    mEdtUsr=mLoginFrame->findChild<QLineEdit *>("edt_usr");
    mEdtPwd=mLoginFrame->findChild<QLineEdit *>("edt_pwd");
    mButAccept=mLoginFrame->findChild<QPushButton *>("accept");
    mButCancel=mLoginFrame->findChild<QPushButton *>("cancel");
    mChkRemember=mLoginFrame->findChild<QCheckBox *>("chk_remember");
    mDefaultPwd=mLoginFrame->findChild<QLabel*>("lab_defaultpwd");

    Q_ASSERT(mEdtUsr&&mEdtPwd&&mButAccept&&mButCancel&&mChkRemember);
    connect(mButAccept,SIGNAL(clicked()),this,SLOT(onLoginUiClicked()));
    connect(mButCancel,SIGNAL(clicked()),this,SLOT(onLoginUiClicked()));
    connect(mChkRemember,SIGNAL(stateChanged(int)),this,SLOT(onChkRememberStateChange(int)));

    QFile cssFile(GENIE2_RES(LOGIN_WIDGET_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        mLoginFrame->setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
//    mLoginFrameProxy=new QGraphicsProxyWidget();
//    mLoginFrameProxy->setWidget(mLoginFrame);
//    mTopologyView->m_scene->addItem(mLoginFrameProxy);
//    mLoginFrameProxy->setZValue(300);
//    mLoginFrameProxy->setVisible(false);



    mDbgLabel=new QLabel();
    mDbgLabel->setFixedSize(250,60);
    mDbgLabel->setWordWrap(true);
//    mDbgLabelProxy=new QGraphicsProxyWidget();
//    mDbgLabelProxy->setWidget(mDbgLabel);
//    mTopologyView->m_scene->addItem(mDbgLabelProxy);
//    mDbgLabelProxy->setZValue(300);
//    mDbgLabelProxy->setPos(-200,-100);
 #ifdef BLOCK_DEVICE_DBG
    mDbgLabel->show();
#endif

   // mLoginFrame->raise();
    //
    //    mCheckBox->hide();

    connect(mControlProxy,SIGNAL(signal_FSMStateChanged(int,int)),
            this,SLOT(onFSMStateChanged(int,int)));
}

BlockDeviceManager::~BlockDeviceManager()
{
    delete mLoginFrame;
    delete mTopLeftW;
    delete mDbgLabel;
}

void BlockDeviceManager::enableDevice(const QString &mac)
{
    enableOrDisable(mac,0);
}

void BlockDeviceManager::disableDevice(const QString &mac)
{
    enableOrDisable(mac,1);
}

void BlockDeviceManager::cancelLoginDialog()
{
    if(mLoginFrame->isVisible())
    {
        mLoginFrame->hide();
        mTopologyView->setEnabled(true);
    }
}

void BlockDeviceManager::retranslateUi()
{

    mButAccept->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_BUT_LEFT));
    mButCancel->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_BUT_RIGHT));
    QLabel *lab_title=mLoginFrame->findChild<QLabel *>("lab_title");
    QLabel *lab_usr=mLoginFrame->findChild<QLabel *>("lab_usr");
    QLabel *lab_pwd=mLoginFrame->findChild<QLabel *>("lab_pwd");
    Q_ASSERT(lab_title && lab_usr && lab_pwd);
    lab_title->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_TITLE));
    lab_usr->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_USR));
    lab_pwd->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_PWD));
    mChkRemember->setText(GENIE2_GET_TEXT(L_MAP_LOGIN_REMEMBER));
    mDefaultPwd->setText(GENIE2_GET_TEXT(L_MAP_DEFAULT_LOGINPWD));

    translateTopLeftMsg();
}

void BlockDeviceManager::resetUi()
{
    if(mTopLeftW)
    {
        mTopLeftW->setVisible(false);
    }
    mFSMState=BLOCKDEVICE_FSM::FS_END;
}

void BlockDeviceManager::raiseLoginFrame()
{
    if(mLoginFrame && mLoginFrame->isVisible())
    {
        mLoginFrame->raise();
    }
}

void BlockDeviceManager::keyEscape()
{
    if(mLoginFrame && mLoginFrame->isVisible())
    {
        mLoginFrame->hide();
        mTopologyView->setEnabled(true);
        bool checked=(mFSMState==BLOCKDEVICE_FSM::FS_RUNNING
                      ||mFSMState==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN);
        mCheckBox->setChecked(checked);
    }

}

void BlockDeviceManager::translateTopLeftMsg()
{
    if(mTopLeftTextIdx<0)
    {
        mTopLeftLabel->setText("");
    }
    else
    {
        mTopLeftLabel->setText(GENIE2_GET_TEXT(mTopLeftTextIdx));
    }
    mCheckBox->setText(GENIE2_GET_TEXT(L_MAP_BLOCKDEVICE_STATE));
}

void BlockDeviceManager::showTopLeftMessage(int lanidx)
{
    bool hasLoginFailed = (mTopLeftTextIdx != L_MAP_BLOCKDEVICE_LOGIN_FAILED)
                        &&(m_oldState == BLOCKDEVICE_FSM::FS_LOGING)
                        &&((mFSMState == BLOCKDEVICE_FSM::FS_RUNNING)
                          ||(mFSMState == BLOCKDEVICE_FSM::FS_CLOSED));

    if(hasLoginFailed){
        QTimer::singleShot(LOGIN_FALED_UI_PRESENTATION_TIME,this,
                           SLOT(onLoginFailedUiTimeout()));
    }

#ifdef SIMPLE_TOPLEFT_TEXT
    lanidx=lanidx>=0?L_MAP_TOPLEFT_PLEASE_WAIT:lanidx;
    if(lanidx >=0 && !hasLoginFailed)
    {
        mPi->startAnimation();
    }
    else
    {
        mPi->stopAnimation();
    }
#endif
    mTopLeftTextIdx= (hasLoginFailed ? L_MAP_BLOCKDEVICE_LOGIN_FAILED : lanidx);
    if(lanidx<0 && !hasLoginFailed)
    {
//        mTopLeftLabelFrame->hide();
//        mCheckBox->show();
        m_panelLayout->setCurrentWidget(mCheckBox);
    }
    else
    {
//        mTopLeftLabelFrame->show();
//        mCheckBox->hide();
        m_panelLayout->setCurrentWidget(mTopLeftLabelFrame);
    }
    /*mTopLeftPanProxy->update();*/
    translateTopLeftMsg();
}

void BlockDeviceManager::enableOrDisable(const QString &mac, int block)
{
    QMap<QString,QVariant> paras;
    paras.insert(mac,block);
    if(mFSMState==BLOCKDEVICE_FSM::FS_RUNNING)
    {
        popLoginWidget(BLOCKDEVICE_FSM::BDC_LOGIN_TO_ENABLINGDISABLING_MAC,paras);
    }
    if(mFSMState==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN)
    {
        mControlProxy->blockDeviceCmd(BLOCKDEVICE_FSM::BDC_ENABLINGDISABLING_MAC,paras);
        showTopLeftMessage(L_MAP_TOPLEFT_SETTING);
    }
}

void BlockDeviceManager::popLoginWidget(int cmd, const QVariant &para)
{
    const QRect &rect1= mLoginFrame->parentWidget()->geometry();
    const QRect &rect2= mLoginFrame->geometry();
    mLoginFrame->move((rect1.width()-rect2.width())>>1
                      ,(rect1.height()-rect2.height())>>1);

    //    mTopologyView->setInteractive(false);
    //    mLoginFrameProxy->setVisible(true);
//    if(mMHandler)
//    {
        mEdtUsr->setText(GENIE2_GET_SETTING(PLUGIN_ROUTECONF_USER).toString());
        mEdtPwd->setText(GENIE2_GET_SETTING(PLUGIN_ROUTECONF_PWD).toString());
        bool bsave=GENIE2_GET_SETTING(PLUGIN_ROUTECONF_SAVEPWD).toBool();
        mChkRemember->setChecked(bsave);

//    }
    mLoginFrame->setFocus();

    mLoginFrame->show();
    mLoginFrame->raise();
    mTopologyView->setEnabled(false);
    //    mLoginFrameProxy->setPos(-rect.width()/2,-rect.height()/2);
    mCmdAfterLoginWidgetOff=cmd;
    mParasAfterLoginWidgetOff=para;
}

bool BlockDeviceManager::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == mLoginFrame && event->type()==QEvent::MouseMove)
    {
        return true;
    }
    return QObject::eventFilter(watched,event);
}

void BlockDeviceManager::onFSMStateChanged(int s,int sold)
{
    mFSMState=s;
    m_oldState = sold;

    //for mLogedIn
    if( (sold==BLOCKDEVICE_FSM::FS_LOGING)
        &&(s!=BLOCKDEVICE_FSM::FS_RUNNING)
            &&(s!=BLOCKDEVICE_FSM::FS_CLOSED) )
    {
        mLogedIn=true;
        if(mChkRemember->isChecked() )
        {
            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_PWD,mEdtPwd->text());
            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_USER,mEdtUsr->text());
        }
        else
        {
            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_PWD,"");
            mEdtPwd->clear();
        }
    }
    else if(s==BLOCKDEVICE_FSM::FS_INIT || s==BLOCKDEVICE_FSM::FS_END)
    {
        mLogedIn=false;
        /*mTopLeftPanProxy->setVisible(false);*/
        mTopLeftW->setVisible(false);
    }


    //for checkbox
    bool topleftpan_show=!(s==BLOCKDEVICE_FSM::FS_INIT ||s==BLOCKDEVICE_FSM::FS_END
                           ||s==BLOCKDEVICE_FSM::FS_GETTING_ATTACHDEVICE
                           ||s==BLOCKDEVICE_FSM::FS_GETTING_ENABLESTAT);
    bool checkBoxEnable=(s==BLOCKDEVICE_FSM::FS_RUNNING
                         || s==BLOCKDEVICE_FSM::FS_CLOSED
                         || s==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN
                         || s==BLOCKDEVICE_FSM::FS_CLOSED_ADMIN);
    bool checkboxChecked=(s==BLOCKDEVICE_FSM::FS_RUNNING
                          || s==BLOCKDEVICE_FSM::FS_RUNNING_ADMIN);
    if(topleftpan_show)
    {
        /*mTopLeftPanProxy->setVisible(true);
#ifdef Q_OS_MACX
    mTopLeftPanProxy->setPos(-316,140+10);
#else
    mTopLeftPanProxy->setPos(-317,140+5);
#endif*/

        mTopLeftW->setVisible(m_ctrPanelCanVisible);
    }

    mCheckBox->setEnabled(checkBoxEnable);
    mCheckBox->setChecked(checkboxChecked);
    const struct _{
        int state;int lanidx;
    } cLanParas[]={
        {BLOCKDEVICE_FSM::FS_GETTING_ATTACHDEVICE,L_MAP_TOPLEFT_UPDATE},
        {BLOCKDEVICE_FSM::FS_RUNNING,-1},
        {BLOCKDEVICE_FSM::FS_RUNNING_ADMIN,-1},
        {BLOCKDEVICE_FSM::FS_CLOSED,-1},
        {BLOCKDEVICE_FSM::FS_CLOSED_ADMIN,-1},
        {BLOCKDEVICE_FSM::FS_LOGING,L_MAP_TOPLEFT_LOGING},
        {BLOCKDEVICE_FSM::FS_ENABLE_DISABLE_MAC,L_MAP_TOPLEFT_SETTING},
        {BLOCKDEVICE_FSM::FS_ENABLING,L_MAP_TOPLEFT_ENABLING},
        {BLOCKDEVICE_FSM::FS_DISABLING,L_MAP_TOPLEFT_DISABLING},
        {BLOCKDEVICE_FSM::FS_END,-1}
    };
    for(int i=0;i<sizeof(cLanParas)/sizeof(cLanParas[0]);i++)
    {
        if(s==cLanParas[i].state)
        {
            showTopLeftMessage(cLanParas[i].lanidx);
            break;
        }
    }



    const char *cStateNames[]={
        "FS_INIT"
        ,"FS_GETTING_ATTACHDEVICE"
        ,"FS_GETTING_ENABLESTAT"
        ,"FS_RUNNING"
        ,"FS_CLOSED"
        ,"FS_RUNNING_ADMIN"
        ,"FS_CLOSED_ADMIN"
        ,"FS_LOGING"
        ,"FS_ENABLING"
        ,"FS_DISABLING"
        ,"FS_ENABLE_DISABLE_MAC"
        ,"FS_END"
    };
    QString stemp=QString("%1 < %2").arg(cStateNames[s],cStateNames[sold]);
    mDbgLabel->setText(stemp);
//    if(mMHandler)
//    {
        GENIE2_LOG(QString("BlockDevice FSM: ")+stemp                                             );
//    }


}

void BlockDeviceManager::onCheckBoxStateChanged(int s)
{
    if(s==Qt::Checked )
     {
         if(mFSMState==BLOCKDEVICE_FSM::FS_CLOSED_ADMIN)
         {
             mControlProxy->blockDeviceCmd(BLOCKDEVICE_FSM::BDC_ENABLE,QString());
             showTopLeftMessage(L_MAP_TOPLEFT_ENABLING);
         }
         if(mFSMState==BLOCKDEVICE_FSM::FS_CLOSED)
         {
             popLoginWidget(BLOCKDEVICE_FSM::BDC_LOGIN_TO_ENABLING);
         }
     }
     if(s==Qt::Unchecked )
     {
         if(mFSMState == BLOCKDEVICE_FSM::FS_RUNNING_ADMIN)
         {
             mControlProxy->blockDeviceCmd(BLOCKDEVICE_FSM::BDC_DISABLE,QString());
             showTopLeftMessage(L_MAP_TOPLEFT_DISABLING);
         }
         else if(mFSMState==BLOCKDEVICE_FSM::FS_RUNNING)
         {
             popLoginWidget(BLOCKDEVICE_FSM::BDC_LOGIN_TO_DISABLING);
         }
//         mControlProxy->blockDeviceCmd(BLOCKDEVICE_FSM::BDC_DISABLE,QString());
     }
}

void BlockDeviceManager::onLoginUiClicked()
{
    qDebug()<<"BlockDeviceManager::onLoginUiClicked";
    QPushButton *btn=qobject_cast<QPushButton *>(sender());
    if(btn==mButAccept)
    {

        QList<QVariant> para;
        para.append(mEdtUsr->text());
        para.append(mEdtPwd->text());
        para.append(mParasAfterLoginWidgetOff);
        mControlProxy->blockDeviceCmd(mCmdAfterLoginWidgetOff,para);
        showTopLeftMessage(L_MAP_TOPLEFT_LOGING);
    }
//    mLoginFrameProxy->setVisible(false);
    keyEscape();
}

void BlockDeviceManager::onChkRememberStateChange(int s)
{
//    if(mMHandler)
//    {
        GENIE2_SET_SETTING(PLUGIN_ROUTECONF_SAVEPWD,mChkRemember->isChecked());
        if(s==Qt::Unchecked )
        {
//            GENIE2_SET_SETTING(PLUGIN_ROUTECONF_PWD,"");
//            mEdtPwd->clear();
        }
//    }
}

void BlockDeviceManager::UpdateMiniOptUiVisible(bool visible)
{
    m_ctrPanelCanVisible = visible;
    bool topleftpan_show=!(mFSMState == BLOCKDEVICE_FSM::FS_INIT || mFSMState == BLOCKDEVICE_FSM::FS_END
                           || mFSMState == BLOCKDEVICE_FSM::FS_GETTING_ATTACHDEVICE
                           || mFSMState == BLOCKDEVICE_FSM::FS_GETTING_ENABLESTAT);

    topleftpan_show=topleftpan_show&&(mDeviceCount>0);

    if(mTopLeftW){
        mTopLeftW->setVisible(m_ctrPanelCanVisible && topleftpan_show);
    }
}

void BlockDeviceManager::onLoginFailedUiTimeout()
{
    if(mTopLeftTextIdx == L_MAP_BLOCKDEVICE_LOGIN_FAILED){
        showTopLeftMessage(-1);
    }
}
