#include "pcpform.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QFrame>
#include "ILanguageManager.h"
#include "LanguageParentalControl.h"
#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QPixmap>
#include <QFrame>
#include "QGeniePluginCableWidget.h"
#include "QGenieDialogContainerFrame.h"
/*
#ifdef Q_WS_WIN
#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#endif
  */

IManagerHandler *PCPForm::sHandler = NULL;

PCPForm::PCPForm(QWidget *parent)
    :QWidget(parent)
{
    bEnabled=true;
    bLog=true;
    filePath = "";
    pathFlag = false;

    this->setObjectName(QString::fromUtf8("mainwidget"));
    setupUi();
    checkPCtrlInstall();
    //SET_FIX_SIZE(NORMAL_BUTTON,l_Info);
    SET_STYLE_PROPERTY(FLEX_BUTTON,l_Info);


}

void PCPForm::Genie_Log(QString s)
{
    if (bLog)
    {
        if (sHandler)
        {
            ILogManager *logmanger_udhbvffd=sHandler->getLogManager();
            if (logmanger_udhbvffd)
            {
                logmanger_udhbvffd->writeLog(s,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}"));
            }
        }
    }
}

void PCPForm::setupUi()
{
    //----create up widget and show information-------------------

    frameAll=new QFrame(this);
    //frameAll->setGeometry(this->geometry());
    frameAll->setObjectName(QString::fromUtf8("frameall"));

    upWidget = new QFrame(frameAll);
    upWidget->setObjectName(QString::fromUtf8("upWidget"));
    //upWidget->setStyleSheet("background-color: rgb(20,80,120);");

    QHBoxLayout *uwLayout = new QHBoxLayout(upWidget);
    uwLayout->setObjectName(QString::fromUtf8("uwLayout"));
    uwLayout->setContentsMargins(30,0,270,0);

    //QFrame* qframe=new QFrame();
    l_Title = new QLabel(upWidget);
    l_Title->setObjectName(QString::fromUtf8("l_Title"));
    l_Title->setWordWrap(true);
   // l_Title->setFixedHeight(140);
    SET_STYLE_PROPERTY(PARENT_LABEL1 ,l_Title) ;

   // uwLayout->insertStretch(0,1);
    uwLayout->addWidget(l_Title);
   // uwLayout->insertStretch(2,50);

    ///////////////////////////////////////
    QWidget* upWidget0;
    upWidget0 = new QFrame(frameAll);
    upWidget0->setObjectName(QString::fromUtf8("upWidget0"));

    QHBoxLayout *uwLayout0 = new QHBoxLayout(upWidget0);
    uwLayout0->setObjectName(QString::fromUtf8("uwLayout0"));
    uwLayout0->setContentsMargins(30,0,260,0);

    //QFrame* qframe=new QFrame();
    l_Live = new QLabel(upWidget0);
    l_Live->setObjectName(QString::fromUtf8("l_Live"));
    l_Live->setWordWrap(false);
    //l_Title->setFrameShape(QFrame::Panel);
    //SET_FIX_SIZE(MAIN_LABEL,l_Live);
    SET_STYLE_PROPERTY(PARENT_LABEL2 ,l_Live) ;
    // SET_STYLE_PROPERTY(PARENT_LABEL3 ,<what>) ;

  //  uwLayout0->insertStretch(0,1);
    uwLayout0->addWidget(l_Live);
  //  uwLayout0->insertStretch(2,1);

    ////////////////////////////////////////

    ///////////////////////////////////////
    QWidget* upWidget1;
    upWidget1 = new QFrame(frameAll);
    upWidget1->setObjectName(QString::fromUtf8("upWidget1"));

    QHBoxLayout *uwLayout1 = new QHBoxLayout(upWidget1);
    uwLayout1->setObjectName(QString::fromUtf8("uwLayout1"));
    uwLayout1->setContentsMargins(30,0,260,0);

    //QFrame* qframe=new QFrame();
    l_One = new QLabel(upWidget1);
    l_One->setObjectName(QString::fromUtf8("l_One"));
    l_One->setWordWrap(true);
    SET_STYLE_PROPERTY(PARENT_LABEL3 ,l_One) ;
    //l_Title->setFrameShape(QFrame::Panel);
    //SET_FIX_SIZE(MAIN_LABEL,l_One);


  //  uwLayout1->insertStretch(0,1);
    uwLayout1->addWidget(l_One);
  //  uwLayout1->insertStretch(2,1);

    ////////////////////////////////////////

    ///////////////////////////////////////
    QWidget* upWidget2;
    upWidget2 = new QFrame(frameAll);
    upWidget2->setObjectName(QString::fromUtf8("upWidget2"));

    QHBoxLayout *uwLayout2 = new QHBoxLayout(upWidget2);
    uwLayout2->setObjectName(QString::fromUtf8("uwLayout2"));
    uwLayout2->setContentsMargins(30,0,260,0);

    //QFrame* qframe=new QFrame();
    l_Two = new QLabel(upWidget2);
    l_Two->setObjectName(QString::fromUtf8("l_Two"));
    l_Two->setWordWrap(true);
    SET_STYLE_PROPERTY(PARENT_LABEL3 ,l_Two) ;
    //l_Title->setFrameShape(QFrame::Panel);
    //SET_FIX_SIZE(MAIN_LABEL,l_Two);


   // uwLayout2->insertStretch(0,1);
    uwLayout2->addWidget(l_Two);
   // uwLayout2->insertStretch(2,1);

    ////////////////////////////////////////

    ///////////////////////////////////////
    QWidget* upWidget3;
    upWidget3 = new QFrame(frameAll);
    upWidget3->setObjectName(QString::fromUtf8("upWidget3"));

    QHBoxLayout *uwLayout3 = new QHBoxLayout(upWidget3);
    uwLayout3->setObjectName(QString::fromUtf8("uwLayout3"));
    uwLayout3->setContentsMargins(30,0,260,0);

    //QFrame* qframe=new QFrame();
    l_Three = new QLabel(upWidget3);
    l_Three->setObjectName(QString::fromUtf8("l_Three"));
    l_Three->setWordWrap(true);
    SET_STYLE_PROPERTY(PARENT_LABEL3 ,l_Three) ;
    //l_Title->setFrameShape(QFrame::Panel);
   // SET_FIX_SIZE(MAIN_LABEL,l_Three);



  //  uwLayout3->insertStretch(0,1);
    uwLayout3->addWidget(l_Three);
   // uwLayout3->insertStretch(2,1);

    ////////////////////////////////////////



    //---create bottom widget for show status and add a button ,startup live parental control tool----
    bottomWidget = new QFrame(frameAll);
    bottomWidget->setObjectName(QString::fromUtf8("bottomWidget"));
    bottomWidget->setMinimumHeight(30);
    //bottomWidget->setStyleSheet("background-color: rgb(20,80,120);");

    QHBoxLayout *bwLayout = new QHBoxLayout(bottomWidget);
    bwLayout->setObjectName(QString::fromUtf8("bwLayout"));
    bwLayout->setContentsMargins(180,0,220,0);

    l_Info = new QPushButton(bottomWidget);
    l_Info->setFocusPolicy(Qt::NoFocus);


    l_Info->setObjectName(QString::fromUtf8("l_Info"));
    SET_STYLE_PROPERTY(FLEX_BUTTON,l_Info);
    l_Info->setEnabled(bEnabled);

    connect(l_Info,SIGNAL(clicked( bool)),this,SLOT(BtnClicked( bool)));

    bwLayout->insertStretch(0,1);
    bwLayout->addWidget(l_Info);
   // bwLayout->addWidget(l_Btn);
    bwLayout->insertStretch(2,300);

    //-----Layout all --------------------------------------

    QVBoxLayout *aLayout = new QVBoxLayout(frameAll);
    aLayout->setObjectName(QString::fromUtf8("aLayout"));
    aLayout->setContentsMargins(0,30,20,110);
    aLayout->setSpacing(5);

    aLayout->insertStretch(0,50);
    aLayout->addWidget(upWidget0);
    aLayout->addWidget(upWidget);
    aLayout->addWidget(upWidget1);
    aLayout->addWidget(upWidget2);
    aLayout->addWidget(upWidget3);
    aLayout->addWidget(bottomWidget);
    aLayout->insertStretch(3,500);

    /*
        #frame
        {
            background-image: url(:/bg_exit.png);
        }
        #label_title
        {
            color:rgb(255,255,255);
            font:bold;
        }
    */
    //QPixmap pixmap(":/bj.png");
    //pixmap.load(strCurrentFileName);
    //this->setPixmap(pixmap);
    //this->setPixmap(QPixmap(":/img/bj.png"));

   // IUiManager *IUiManager1=PCPForm::sHandler->getUiManager();
   // IUiManager1->setBackGroundImg(QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}"),":/img/bj.png");
   // l_Title->setText(lanmanager->getText(L_PARENTAL_CONTROL_DESC,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    frameAll->setStyleSheet("#frameall{background-image: url(:/img/bj.png);}");

    retranslateUi();
}

void PCPForm::retranslateUi()
{

    /*
    QString t_Text = "Live Parental Controls powered by OpenDNS is a free add-on feature to many NETGEAR "
                     "products. It creates a safer Internet experience by blocking inappropriate content from "
                    "50+ categories and provides protections to all connectivity devices in the houseold.The "
                    "settings are easily customizable through a web interface which allows you to remotely manage "
                    "home security settings from anywhere in ths world.";    
    */
 /*        L_PARENTAL_CONTROL_BIGBT=1300,
        L_PARENTAL_CONTROL_LEFTBT,
        L_PARENTAL_CONTROL_DESC,
        L_PARENTAL_CONTROL_MANAGER,
        L_PARENTAL_CONTROL_INSTALL
 */
    ILanguageManager *lanmanager=PCPForm::sHandler->getLanguageManager();
    l_Title->setText(lanmanager->getText(L_PARENTAL_CONTROL_DESC,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    /*
        L_PARENTAL_CONTROL_FUN_ONE,
        L_PARENTAL_CONTROL_FUN_TWO,
        L_PARENTAL_CONTROL_FUN_THREE,
        L_PARENTAL_CONTROL_LIVE
    */

    l_Live->setText(lanmanager->getText(L_PARENTAL_CONTROL_LIVE,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    l_One->setText(lanmanager->getText(L_PARENTAL_CONTROL_FUN_ONE,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    l_Two->setText(lanmanager->getText(L_PARENTAL_CONTROL_FUN_TWO,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    l_Three->setText(lanmanager->getText(L_PARENTAL_CONTROL_FUN_THREE,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));

    //l_Title->setText(t_Text);
    if (bInstall)
    {
        l_Info->setText(lanmanager->getText(L_PARENTAL_CONTROL_INSTALL,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
        //l_Info->setEnabled(true);
    }
    else
    {
        l_Info->setText(lanmanager->getText(L_PARENTAL_CONTROL_MANAGER,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));

    }

    //BBT_PARA_GOOD_TEXT_RIGHT
     l_Info->setEnabled(bEnabled);

}

void PCPForm::checkPCtrlInstall()
{

    //QString filePath;
    //bool pathFlag;
    filePath.clear();

//    #ifdef Q_WS_WIN
//	int len =0;
//    wchar_t szDstFile[MAX_PATH]={0};
//    GetSystemDirectory(szDstFile,MAX_PATH);
//	for(int i = 0; i<MAX_PATH; i++)
//	{
//		if(szDstFile[i] == 0)
//		{
//			len = i;
//			break;
//		}
//	}
//        filePath = QString::fromWCharArray(szDstFile,len);
//        filePath.trimmed();
//
//    #endif
//

    filePath = QDir::rootPath();
    filePath.trimmed();

    filePath = filePath.left(filePath.indexOf("/",0));
    filePath32 =filePath + "/Program Files/NETGEAR Live Parental Controls Management Utility";
    filePath64 =filePath + "/Program Files (x86)/NETGEAR Live Parental Controls Management Utility";
    QDir dir(filePath32);
    QDir dir64(filePath64);
    if(dir.exists() || dir64.exists())
    {
        //l_Info->setText(tr("Manager"));
        //l_Info->setEnabled(true);
        bEnabled=true;
        bInstall=false;
        pathFlag = true;
    }
    else
    {
        //l_Info->setText(tr("Install"));
        bInstall=true;
        //NETGEARManagementUtility-2.1-install.exe
        QFile file(INSTALL_FILE_NAME);
        if(!file.exists())
        {
           // l_Info->setEnabled(false);
            bEnabled=false;
        }
        else
        {
            //l_Info->setEnabled(true);
            bEnabled=true;
        }
    }
    retranslateUi();
}

void PCPForm::BtnClicked( bool checked  )
{

    IClickManager *clickmanager=PCPForm::sHandler->getClickManager();


    checkPCtrlInstall();
    if(!pathFlag)
    {

        Genie_Log("install");
        execInstall();
        if (clickmanager)
        {
            clickmanager->click("P1");
        }
        checkPCtrlInstall();
        retranslateUi();
        return ;
    }
    QString execFile = filePath32 + "/NetgearManagementUtility.exe";
    QFile file(execFile);
    if(file.exists())
    {
        QProcess *pro = new QProcess(this);
        QStringList list;
        list << "";

        Genie_Log("manager");
        if (clickmanager)
        {
            clickmanager->click("P2");
        }

        pro->start(execFile,list);
    }
    else
    {
        QString execFile1 = filePath64 + "/NetgearManagementUtility.exe";
        QFile file1(execFile1);
        if(file1.exists())
        {

            QProcess *pro1 = new QProcess(this);
            QStringList list1;
            list1 << "";

            Genie_Log("manager");

            if (clickmanager)
            {
                clickmanager->click("P2");
            }
            pro1->start(execFile1,list1);
        }
        else
        {

            Genie_Log("install");
            if (clickmanager)
            {
                clickmanager->click("P1");
            }
            execInstall();
        }
    }
    checkPCtrlInstall();
    retranslateUi();
}

void PCPForm::resizeEvent(QResizeEvent *)
{
   //l_Title->setMaximumWidth(this->size().width()-200);
  // l_Info->setMinimumWidth(this->size().width()-350);

     frameAll->setGeometry(0,0,this->geometry().width()-3,this->geometry().height()+8);
     //frameAll->setGeometry(0,0,500,600);
}

void PCPForm::execInstall()
{

    QProcess::startDetached(INSTALL_FILE_NAME);
    IUiManager *mUiPage = PCPForm::sHandler->getUiManager();
    Q_ASSERT(mUiPage);
    mUiPage->changeStackIdx(0);

}


void PCPForm::changeEvent(QEvent *e)
{
   // ILanguageManager *lanmanager=WorkPanel::sHandler->getLanguageManager();
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
        retranslateUi();
        break;
    default:
        break;
    }
}

void	PCPForm::showEvent ( QShowEvent * event )
{
    checkPCtrlInstall();
    retranslateUi();
    if (l_Info)
    l_Info->setEnabled(bEnabled);

}

 void	PCPForm::focusInEvent ( QFocusEvent * event )
 {
     checkPCtrlInstall();
     retranslateUi();
     if (l_Info)
     l_Info->setEnabled(bEnabled);
 }
