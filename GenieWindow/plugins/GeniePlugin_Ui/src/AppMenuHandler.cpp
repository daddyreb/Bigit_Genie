#include "AppMenuHandler.h"
//#include "QGeniePrefrencesPanel.h"

#include "genie2_interface.h"
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QWidget>
#include <QUiLoader>
#include "language_Ui.h"
#include "QGenieUiObject.h"
#include "QGeniePrivilegeProcess.h"
#include <QProcess>

AppMenuHandler* AppMenuHandler::g_handler = 0;
//QGeniePrefrencesPanel* AppMenuHandler::m_prefrencesPanel = 0;
#define SYSTEM_AGENT_PATH "/Library/LaunchAgents"
#define STARTUP_AGENT "com.netgear.NETGEARGenie.plist"


AppMenuHandler::AppMenuHandler(QObject *parent) :
        QObject(parent)
{
    m_appMenuBar = new QMenuBar();

    QMenu *appMenu = new QMenu(0);
    m_appMenuBar->addMenu(appMenu);

    m_actAbout = new QAction("About NETGEAR Genie",0);
    m_actAbout->setData(int(APPMENU_ABOUT));
    m_actAbout->setMenuRole(QAction::ApplicationSpecificRole);
    appMenu->addAction(m_actAbout);

    QAction * act = new QAction("Settings",0);
    act->setData(int(APPMENU_PREFERENCES));
    act->setMenuRole(QAction::PreferencesRole);
    appMenu->addAction(act);

    act = new QAction("Check For Updates...",0);
    act->setData(int(APPMENU_CHECKFORUPDATE));
    act->setMenuRole(QAction::ApplicationSpecificRole);
    appMenu->addAction(act);
    m_actUpdate = act;

    act = new QAction("Quit",0);
    act->setData(int(APPMENU_QUIT));
    act->setMenuRole(QAction::QuitRole);
    appMenu->addAction(act);

    connect(appMenu,SIGNAL(triggered(QAction*)),this,SLOT(triggered(QAction*)));
    mProcess=new QProcess(this);
}


AppMenuHandler::~AppMenuHandler()
{
  //  delete m_prefrencesPanel;
    delete m_appMenuBar;
}

void AppMenuHandler::triggered(QAction *act)
{
    if(act)
    {
        bool bok = false;
        int actData = act->data().toInt(&bok);
        if(bok)
        {
            switch(actData)
            {
            case APPMENU_ABOUT:
                about();
                break;
            case APPMENU_CHECKFORUPDATE:
                checkForUpdate();
                break;
            case APPMENU_PREFERENCES:
                prefrences();
                break;
            case APPMENU_QUIT:
                quit();
                break;
            }
        }
    }
}

void AppMenuHandler::retranslateUi()
{
    m_actAbout->setText(GENIE2_GET_TEXT(L_UI_ABOUT_TITLE)+" NETGEAR Genie");
        m_actUpdate->setText(GENIE2_GET_TEXT(L_UI_CHECKFORUPDATES)+"...");
}

void AppMenuHandler::about()
{
    QGenieUiObject *uiobj=QGenieUiObject::GetSingletonPtr();
    if(uiobj)
    {
        uiobj->_showAboutDialog();
    }
    //todo 1111
//    QGenieFrameTitle *frameTitle = QGenieFrameTitle::GetSingletonPtr();
//    frameTitle->on_but_about_clicked();
}

void AppMenuHandler::checkForUpdate()
{
    QGenieUiObject *uiobj=QGenieUiObject::GetSingletonPtr();
    //check if plugin loaded
    GENIE2_HANDLER_CMD(HCMD_LOADPLUGIN,PLUGIN_FILE_UPDATE);
    //tell message to plugin
    GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_UPDATE, EPCC_SOFTWARE_UPDATE, (uiobj?uiobj->mCurLan:"enu"));

}

void AppMenuHandler::prefrences()
{
    QUiLoader loader;
    QFile dlg_file(GENIE2_RES("ui/macsetting.ui"));
    bool btemp=dlg_file.open(QFile::ReadOnly);
    Q_ASSERT(btemp);
    QWidget *w=loader.load(&dlg_file, QGenieUiObject::GetSingleton().mMainFrame);

    QDialog *dlg=static_cast<QDialog *>(w);
    QGenieUiObject::GetSingleton()._setFramelessStyle(dlg);

    const struct _{const char *name;int lan;int type;}
    cPara[]={
        {"chk0", L_UI_TRAY_AUTOSTART, 2}
        ,{"chk1",L_UI_TRAY_NETCONNECTION_CHANGE,2}
        ,{"ok",L_UI_OK,1}
        ,{"cancel",L_UI_CANCEL,1}

    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_LABEL(w,cPara[i].name,cPara[i].lan);
        }
        if(1==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_BUTTON(w,cPara[i].name,cPara[i].lan);
        }
        if(2==cPara[i].type)
        {
            GENIE2_TRANSLATE_CHILD_CHECKBOX(w,cPara[i].name,cPara[i].lan);
        }
    }

    QList<QPushButton *> bts=w->findChildren<QPushButton *>();
    int c=bts.count();
    for(int i=0;i<c;i++)
    {
        connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_SettingButClicked()));
    }
//    QDir d(SYSTEM_AGENT_PATH);
    QCheckBox *chkAutoStart=dlg->findChild<QCheckBox *>("chk0");
    QCheckBox *chkShowNetworkChange=dlg->findChild<QCheckBox *>("chk1");
    Q_ASSERT(chkAutoStart && chkShowNetworkChange);

    bool autostartOn=false;
    QProcess process;
    process.start(qApp->applicationDirPath()+"/LoginItem",QStringList()<<"2");
    if(process.waitForFinished(800))
    {
        if(process.exitCode()==1)
        {
            autostartOn=true;
        }
    }
    chkAutoStart->setChecked(autostartOn);
    chkShowNetworkChange->setChecked(GENIE2_GET_SETTING(SHOW_NETWORK_CHANGE).toBool());


    if(1==dlg->exec())
    {

        QString agent_path=qApp->applicationDirPath()+"/../Resources/"STARTUP_AGENT;


        if(chkAutoStart->isChecked() && !autostartOn)
        {
            if(!mProcess->waitForFinished(10))
            {
                mProcess->kill();
            }
            mProcess->start(qApp->applicationDirPath()+"/LoginItem",QStringList()<<"1");
//            mPrivilegeProcess->waitForFinished();
        }
        else if(!chkAutoStart->isChecked() && autostartOn)
        {
            if(!mProcess->waitForFinished(10))
            {
                mProcess->kill();
            }
            mProcess->start(qApp->applicationDirPath()+"/LoginItem",QStringList()<<"0");
//            mPrivilegeProcess->waitForFinished();
        }

       // GENIE2_SET_SETTING(MACOS_AUTOSTART,chkAutoStart->isChecked());
        GENIE2_SET_SETTING(SHOW_NETWORK_CHANGE,chkShowNetworkChange->isChecked());
    }
    GENIE2_SAFEDELETE(dlg);
}

void AppMenuHandler::quit()
{
    QGenieUiObject *uiobj=QGenieUiObject::GetSingletonPtr();
    if(uiobj)
    {
        QTimer::singleShot(0,uiobj,SLOT(slot_Exit()));
    }
}

void AppMenuHandler::installAppMenu()
{
    if(0 == g_handler)
    {
        g_handler = new AppMenuHandler();
    }
}

void AppMenuHandler::uninstallAppMenu()
{
    delete g_handler;
    g_handler = 0;
}

//void AppMenuHandler::s_retranslateUi()
//{
//    if(g_handler)
//    {
//        g_handler->retranslateUi();
//        if(!m_prefrencesPanel)
//        {
//            m_prefrencesPanel = new QGeniePrefrencesPanel();
//            connect(m_prefrencesPanel,SIGNAL(updateLanguage()),g_handler,SLOT(retranslateUi()));
//           // m_prefrencesPanel->hide();
//        }
//    }
//}
