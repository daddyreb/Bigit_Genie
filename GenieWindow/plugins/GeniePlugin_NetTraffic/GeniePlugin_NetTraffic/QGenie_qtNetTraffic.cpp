#include "QGenie_qtNetTraffic.h"
#include "QGenie_qtNetStat.h"
#include "QGenie_qtGraphColor.h"
#include "QGenie_qtOptions.h"
#include <QtGui/QTreeWidgetItem>
#include <QBrush>
#include <QString>
#include <QFont>
#include <QtGui/QScreen>
#include <QDesktopWidget>
#include <QRect>
#include "IInfoManager.h"

#include "IUiManager.h"
#include "IPluginInf.h"
#include "ILanguageManager.h"
#include "LanguageNetTraffic.h"
#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "QGenieAboutThread.h"
#include <QUuid>
#include <QtGlobal>
#include "IUiManager.h"


IManagerHandler *QGenie_qtNetTraffic::sHandler=NULL;

QGenie_qtNetTraffic::QGenie_qtNetTraffic(QWidget *parent)
    : QWidget(parent)
{
    bLog=true;
    bNetOk=true;
    this->setWindowIcon(QIcon(":/image/aa.ico"));
    IInfoManager* Iinfo=sHandler->getInfoManager();

    QRect rect=Iinfo->getGeometry(INFO_CMD_MAIN_GEOMETRY);
    int x=rect.x();
    int y=rect.y();
    int w=rect.width();
    int h=rect.height();
    int sw=2*x+w;
    int sh=2*y+h;
    this->setGeometry(sw-200,sh-90,150,110);


    //

    this->setMinimumWidth(150);
    this->setMinimumHeight(50);
    iMode=0;
    /*
    QFont qft=QFont();

    qft.setFamily(QString("MS Sans Serif"));
    qft.setBold(true);
    this->setFont(qft);

    */

    this->setStyleSheet("font: 12px;\ncolor: black;");
    bFontBold=false;

    qrectRestored=this->geometry();


    Qt::WindowFlags flags = 0;
    flags=Qt::CustomizeWindowHint;
    flags |= Qt::WindowStaysOnTopHint;
    //flags |=Qt::FramelessWindowHint;
    flags |=Qt::Tool;
    this->setWindowFlags(flags);


    lblInput=new QLabel(this);
    lblInput->setPixmap(QPixmap(":/image/download.png"));
    lblInput->setGeometry(3,114,12,16);
    lblInput->setToolTip(QString("Download"));

    txtInput=new   QLabel(this);

    txtInput->setGeometry(15,114,57,16);
    txtInput->setTextFormat(Qt::PlainText);
    txtInput->setStyleSheet("font: 11px;\ncolor: black;");


    //QLabel* lblAlert;
    //QLabel* txtAlert;

    lblAlert=new QLabel(this);
    lblAlert->setPixmap(QPixmap(":/image/alert.png"));
    lblAlert->setGeometry(3,114,12,16);
    lblAlert->setToolTip(QString("Download"));
    lblAlert->setVisible(false);

    txtAlert=new   QLabel(this);
    txtAlert->setGeometry(15,114,57,16);
    txtAlert->setTextFormat(Qt::PlainText);
    txtAlert->setStyleSheet("font: 11px;\ncolor: black;");
    txtAlert->setVisible(false);


    lblOutput=new QLabel(this);
    lblOutput->setPixmap(QPixmap(":/image/upload.png"));
    lblOutput->setGeometry(79,114,12,16);
    lblOutput->setToolTip(QString("Upload"));

    txtOutput=new   QLabel(this);


    txtOutput->setGeometry(91,114,57,16);
    txtOutput->setTextFormat(Qt::PlainText);
    txtOutput->setStyleSheet("font: 11px;\ncolor: black;");


    Timerdraw=new QTimer();
    Timerdraw->setInterval(1000);
    Timerdraw->start();
   // connect(Timerdraw,SIGNAL(timeout ()),this,SLOT(TimerDrawtimeout ()));
    ICountTimer=0;

    brushInitalized = FALSE;
    //interfaceCallBack = NULL;
    useAdaptiveScale = FALSE;
    MaxTrafficAmount = 0.0;
    SelectedInterface = 0;
    gridxstartpos = 0;
    gridystartpos = 0;
    gridxresolution		=	GRIDXRESOLUTION;
    gridyresolution		=	GRIDYRESOLUTION;
    gridscrollxspeed            =	GRIDSCROLLXSPEED;
    gridscrollyspeed            =	GRIDSCROLLYSPEED;
    plotgranularity		=	PLOTGRANULATRITY;
    netupdatespeed		=	NETUPDATESPEED;
    gridupdatespeed		=	GRIDUPDATESPEED;
    ToggleStatusText            =	"局部最大值";

    TrafficEntries=620;
    //TrafficEntries = TGSize.cx / plotgranularity;
    TrafficStats = new TRAFFICENTRY[TrafficEntries+1];
    TrafficStatsIncoming=new TRAFFICENTRY[TrafficEntries+1];
    TrafficStatsOutgoing=new TRAFFICENTRY[TrafficEntries+1];

    for(DWORD x=0; x<TrafficEntries+1; x++)
    {
            TrafficStats[x].connected = TRUE;
            TrafficStats[x].value	= 0.0;
            TrafficStatsIncoming[x].connected = TRUE;
            TrafficStatsIncoming[x].value	= 0.0;
            TrafficStatsOutgoing[x].connected = TRUE;
            TrafficStatsOutgoing[x].value	= 0.0;

    }

    qclrReceive=QColor(0,131,215);
    qclrSent=QColor(0,153,0);
    qclrTotal=QColor(255,204,0);
    //qclrWindowBg=QColor(222,255,255);
    qclrGraphBg=QColor(222,255,255);
    qclrWindowBg=QColor(180,226,255);
    //qclrWindowBg=QColor(153,204,255);


    dbOpt=0.8;
    this->setWindowOpacity(dbOpt);

    iunit=0;
    iShowUnit=1;
    bFontBold=false;
    for (int i=0;i<3;i++)
    {
        infChoose[i]=1;
    }

    qfrPtn=new QFrame(this);
    ptnMin=new QPushButton(qfrPtn);
    ptnClose=new QPushButton(qfrPtn);
    ptnNormal=new QPushButton(qfrPtn);
    qfrPtn->setGeometry(this->width()-60,5,60,20);
    ptnMin->setGeometry(2,1,16,16);
    ptnNormal->setGeometry(21,1,16,16);
    ptnClose->setGeometry(40,1,16,16);

    ptnMin->setIconSize(QSize(16,16));
    ptnNormal->setIconSize(QSize(16,16));
    ptnClose->setIconSize(QSize(16,16));
    //ptnMin->setStyleSheet();
    ptnMin->setIcon(QIcon(":/image/mini.ico"));
    ptnNormal->setIcon(QIcon(":/image/normal.ico"));
    ptnClose->setIcon(QIcon(":/image/close.ico"));

    ptnNormal->setVisible(false);
    ptnClose->setVisible(false);
    ptnMin->setVisible(false);

    connect(ptnMin,SIGNAL(clicked ( bool  )),this,SLOT(minClicked (bool)));
    connect(ptnClose,SIGNAL(clicked ( bool  )),this,SLOT(closeClicked (bool)));
    connect(ptnNormal,SIGNAL(clicked ( bool  )),this,SLOT(normalClicked (bool)));

    txtInput->setAlignment(Qt::AlignCenter);
    txtOutput->setAlignment(Qt::AlignCenter);
    txtAlert->setAlignment(Qt::AlignCenter);

    leftkeypressed=0;
    createTrayIcon();

    retranslateUi();

    bOptPoped=false;
    bColorPoped=false;
    bStatPoped=false;

    minClicked(false);

    //QSettings setting(ORGANIZATION_NAME,APPNAME);
    //bool b=setting.value(SETTING_TRAFFIC_START_MINIMIZE,false).toBool();

     iDrawCount=this->width()-20;

     //IUiManager
     IUiManager * iUi=sHandler->getUiManager();
     if (iUi)
     {
         iUi->setNetStateFlag( QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"),true);
     }

    //painter=new QPainter(this);
     lbl_internet_input=0;
     lbl_internet_output=0;
}

void QGenie_qtNetTraffic::Genie_Log(QString s)
{
    if (bLog)
    {
        if (sHandler)
        {
            ILogManager *logmanger_udhbvffd=sHandler->getLogManager();
            if (logmanger_udhbvffd)
            {
                logmanger_udhbvffd->writeLog(s,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
            }
        }
    }
}


void QGenie_qtNetTraffic::retranslateUi()
{
     //Genie_Log("retranslateUi()");
    //ILanguageManager *lanmanager=QGenie_qtNetTraffic::sHandler->getLanguageManager();
    //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
    /*
        L_NETTRAFFIC_MENU_NET_TRAFFIC,
        L_NETTRAFFIC_MENU_SWITCH_MODE,
        L_NETTRAFFIC_MENU_SWITCH_FONT,
        L_NETTRAFFIC_MENU_GENERAL_OPTIONS,
        L_NETTRAFFIC_MENU_GRAPH_COLOR,
        L_NETTRAFFIC_MENU_NETSTAT_LIST

    QString strMenuNormal;
    QString strMenuFont;
    QString strMenuNetchoose;
    QString strMenuGraphColor;
    QString strMenuNetStat;
    QString strMenuTroy;

    QAction* actNormal;
    QAction* actFont;
    QAction* actNetchoose;
    QAction* actGraphColor;
    QAction* actNetStat;
    QMenu* qmenuTroy;



    strMenuNormal=lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_MODE,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    strMenuFont=lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_FONT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    strMenuNetchoose=lanmanager->getText(L_NETTRAFFIC_MENU_GENERAL_OPTIONS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    strMenuGraphColor=lanmanager->getText(L_NETTRAFFIC_MENU_GRAPH_COLOR,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    strMenuNetStat=lanmanager->getText(L_NETTRAFFIC_MENU_NETSTAT_LIST,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    strMenuTroy=lanmanager->getText(L_NETTRAFFIC_MENU_NET_TRAFFIC,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"));
    //L_NETTRAFFIC_ALERT_DISCON
    //lblAlert->setVisible(false);

*/

    /*
    actNormal->setText(lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_MODE));
    actFont->setText(lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_FONT));
    actNetchoose->setText(lanmanager->getText(L_NETTRAFFIC_MENU_GENERAL_OPTIONS));
    actGraphColor->setText(lanmanager->getText(L_NETTRAFFIC_MENU_GRAPH_COLOR));
    actNetStat->setText(lanmanager->getText(L_NETTRAFFIC_MENU_NETSTAT_LIST));


    qmenuTroy->setTitle(lanmanager->getText(L_NETTRAFFIC_MENU_NET_TRAFFIC,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));

    actTrayNormal->setText(lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_MODE,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    actTrayFont->setText(lanmanager->getText(L_NETTRAFFIC_MENU_SWITCH_FONT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    actTrayNetchoose->setText(lanmanager->getText(L_NETTRAFFIC_MENU_GENERAL_OPTIONS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    actTrayGraphColor->setText(lanmanager->getText(L_NETTRAFFIC_MENU_GRAPH_COLOR,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    actTrayNetStat->setText(lanmanager->getText(L_NETTRAFFIC_MENU_NETSTAT_LIST,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    txtAlert->setText(lanmanager->getText(L_NETTRAFFIC_ALERT_DISCON,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
     */
}


void QGenie_qtNetTraffic::changeEvent(QEvent *e)
{
     //Genie_Log("changeEvent()");
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


QGenie_qtNetTraffic::~QGenie_qtNetTraffic()
{
    Genie_Log("~QGenie_qtNetTraffic()");
    if (Timerdraw)
    {
        Timerdraw->deleteLater();
    }
}

void QGenie_qtNetTraffic::createTrayIcon()
{

    //Genie_Log("createTrayIcon()");
    //qmenuTroy
    qmenuTroy=new QMenu(QString("Net Traffic"),this);
    //SET_STYLE_PROPERTY (TRAY_MENU,qmenuTroy);

    qmenuTroy->setIcon(QIcon(":/image/Net Traffic.png"));

    actTrayNormal=new QAction("Switch Mode",0);
   // qmenuTroy->addAction(actTrayNormal);

    /*
    QAction* actMini=new QAction("Mini Mode",0);
    qmenuTroy->addAction(actMini);
    */

   // qmenuTroy->addSeparator();


    actTrayFont=new QAction("Switch Font",0);
    //qmenuTroy->addAction(actTrayFont);

   // qmenuTroy->addSeparator();

    actTrayNetchoose=new QAction("General Options",0);
   // qmenuTroy->addAction(actTrayNetchoose);

    actTrayGraphColor=new QAction("Graph Color",0);
   // qmenuTroy->addAction(actTrayGraphColor);

    actTrayNetStat=new QAction("NetStat List",0);
  //  qmenuTroy->addAction(actTrayNetStat);

   // qmenuTroy->addSeparator();
   // QAction* actExit=new QAction("Exit",0);
  //  qmenuTroy->addAction(actExit);

    connect(qmenuTroy,SIGNAL(triggered(QAction*)),this,SLOT(popMenuClicked(QAction*)));
    leftkeypressed=0;

   // trayIcon = new QSystemTrayIcon(this);
    //trayIcon->setIcon(QIcon(":/image/aa.ico"));
   // trayIcon->setContextMenu(popMenu);
}

void QGenie_qtNetTraffic::minClicked(bool checked)
{

   // Genie_Log("minClicked()");
    if (iMode==0)
    {
        ptnNormal->setVisible(false);
        ptnClose->setVisible(false);
        ptnMin->setVisible(false);
        qrectRestored=this->geometry();

        if (iShowUnit==1)
        {
            this->setFixedSize(150,17);

            lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
        }
        else
        {
            this->setFixedSize(100,17);

            lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);

        }

        iMode=1;

        Qt::WindowFlags flags = 0;
        flags=Qt::CustomizeWindowHint;
        flags |= Qt::WindowStaysOnTopHint;
        flags |=Qt::FramelessWindowHint;
        flags |=Qt::Tool;

        this->setWindowFlags(flags);
       // this->show();
        this->hide();
    }
}

void QGenie_qtNetTraffic::closeClicked(bool checked)
{
   // Genie_Log("closeClicked()");
    this->hide();
}

void QGenie_qtNetTraffic::normalClicked(bool checked)
{
   // Genie_Log("normalClicked()");
    if (iMode==1)
    {
        iMode=0;

        Qt::WindowFlags flags = 0;
        flags=Qt::CustomizeWindowHint;
        flags |= Qt::WindowStaysOnTopHint;
        //flags |=Qt::FramelessWindowHint;
        flags |=Qt::Tool;

        this->setWindowFlags(flags);
        //show();
        this->hide();
        this->setMaximumSize(2000,2000);
        this->setMinimumWidth(150);
        this->setMinimumHeight(50);
        this->setGeometry(this->x(),this->y(),qrectRestored.width(),qrectRestored.height());


        if (qrectRestored.width()>149 && qrectRestored.height()>15 )
        {

            lblInput->setGeometry(3,qrectRestored.height()-16,12,16);
            txtInput->setGeometry(15,qrectRestored.height()-16,qrectRestored.width()/2-15-3,16);
            lblOutput->setGeometry(qrectRestored.width()/2+3,qrectRestored.height()-16,12,16);
            txtOutput->setGeometry(qrectRestored.width()/2+15,qrectRestored.height()-16,qrectRestored.width()/2-15-3,16);
        }

        ptnNormal->setVisible(false);
        ptnClose->setVisible(false);
        ptnMin->setVisible(false);

    }
}

 void QGenie_qtNetTraffic::enterEvent ( QEvent * event )
{

     // Genie_Log("enterEvent()");
     if (iMode==0)
     {
        ptnNormal->setVisible(false);
        ptnClose->setVisible(true);
        ptnMin->setVisible(true);
     }
     else
     {
         if (iShowUnit==1)
         {
            this->setFixedSize(190,17);
         }
         else
         {
              this->setFixedSize(140,17);
         }

         lblInput->setGeometry(3,this->geometry().height()-16,12,16);
         txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-20-3,16);
         lblOutput->setGeometry(this->geometry().width()/2-20+3,this->geometry().height()-16,12,16);
         txtOutput->setGeometry(this->geometry().width()/2-20+15,this->geometry().height()-16,this->geometry().width()/2-35-3,16);


         qfrPtn->setGeometry(this->width()-60,0,60,18);
         ptnNormal->setVisible(true);
         ptnClose->setVisible(true);
         ptnMin->setVisible(false);
     }

     QWidget::enterEvent(event);

}

 void	QGenie_qtNetTraffic::leaveEvent (  QEvent * event )
 {

    // Genie_Log("leaveEvent()");
     if (iMode==0)
     {
         ptnNormal->setVisible(false);
         ptnClose->setVisible(false);
         ptnMin->setVisible(false);
     }
     else
     {
          if (iShowUnit==1)
          {
              this->setFixedSize(150,17);
          }
          else
          {
              this->setFixedSize(100,17);
          }

         lblInput->setGeometry(3,this->geometry().height()-16,12,16);
         txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
         lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
         txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);



         ptnNormal->setVisible(false);
         ptnClose->setVisible(false);
         ptnMin->setVisible(false);
     }
     QWidget::leaveEvent(event);
 }

void	QGenie_qtNetTraffic::resizeEvent ( QResizeEvent * event )
{
   // Genie_Log("resizeEvent()");

    if (iMode==0)
    {
        if (this->width()>90)
        {
            qfrPtn->setGeometry(this->width()-60,5,60,20);
        }
        else
        {
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
        }
        if (this ->height()<50)
        {
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
        }

        lblAlert->setVisible(false);


        txtAlert->setVisible(false);

    }
    else
    {
        ptnNormal->setVisible(false);
        ptnClose->setVisible(false);
        ptnMin->setVisible(false);

    }

   // qrectRestored=this->geometry(); this

    if (this->geometry().width()>99 && this->geometry().height()>15 )
    {

        lblInput->setGeometry(3,this->geometry().height()-16,12,16);
        txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
        lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
        txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
       // if ()
        lblAlert->setGeometry(3,this->geometry().height()-16,16,16);
        txtAlert->setGeometry(20,this->geometry().height()-16,130,16);

    }

     iDrawCount=this->width()-20;
}


void QGenie_qtNetTraffic::mousePressEvent ( QMouseEvent * event )
{
    //生成菜单
    // Genie_Log("mousePressEvent()");
    if (event->button () ==Qt::RightButton)
    {

        /*
        L_NETTRAFFIC_MENU_NET_TRAFFIC,
        L_NETTRAFFIC_MENU_SWITCH_MODE,
        L_NETTRAFFIC_MENU_SWITCH_FONT,
        L_NETTRAFFIC_MENU_GENERAL_OPTIONS,
        L_NETTRAFFIC_MENU_GRAPH_COLOR,
        L_NETTRAFFIC_MENU_NETSTAT_LIST
        */

        QMenu* popMenu=new QMenu(this);

        QAction* actNormal;
        QAction* actFont;
        QAction* actNetchoose;
        QAction* actGraphColor;
        QAction* actNetStat;

        /*
        QString strMenuNormal;
        QString strMenuFont;
        QString strMenuNetchoose;
        QString strMenuGraphColor;
        QString strMenuNetStat;
        */

        actNormal=new QAction(strMenuNormal,0);
        actFont=new QAction(strMenuFont,0);
        actNetchoose=new QAction(strMenuNetchoose,0);
        actGraphColor=new QAction(strMenuGraphColor,0);
        actNetStat=new QAction(strMenuNetStat,0);

        popMenu->addAction(actNormal);
        popMenu->addSeparator();
        popMenu->addAction(actFont);
        popMenu->addSeparator();
        popMenu->addAction(actNetchoose);
       // popMenu->addAction(actGraphColor);
       // popMenu->addAction(actNetStat);
/*
        popMenu->addSeparator();
        QAction* actExit=new QAction("Exit",0);
        popMenu->addAction(actExit);
*/
        connect(popMenu,SIGNAL(triggered(QAction*)),this,SLOT(popMenuClicked(QAction*)));
        leftkeypressed=0;
        popMenu->popup(event->globalPos());

    }
    if (event->button () ==Qt::LeftButton)
    {
        leftkeypressed=1;
        xMouse=event->x();
        yMouse=event->y();
    }
    QWidget::mousePressEvent(event);
}


void  QGenie_qtNetTraffic::mouseMoveEvent ( QMouseEvent * event )
{

    //Genie_Log("mousePressEvent()");
    if (leftkeypressed==1)
    {
        int x=event->globalX()-xMouse;
        int y=event->globalY()-yMouse;
        this->setGeometry(x,y,this->geometry().width(),this->geometry().height());
    }
    QWidget::mouseMoveEvent(event);
}
void  QGenie_qtNetTraffic::mouseReleaseEvent ( QMouseEvent * event )
{
    leftkeypressed=0;
    QWidget::mouseReleaseEvent(event);
}

void QGenie_qtNetTraffic::popMenuClicked(QAction* action)
{

    /*
    QString strMenuNormal;
    QString strMenuFont;
    QString strMenuNetchoose;
    QString strMenuGraphColor;
    QString strMenuNetStat;
    QString strMenuTroy;
    */

    Genie_Log("popMenuClicked()");

    IClickManager *clickmanager=QGenie_qtNetTraffic::sHandler->getClickManager();
    if (clickmanager)
    {
        clickmanager->click("j");
    }

    if (action->text()==strMenuNormal)
    {
        if (iMode==1)
        {
            iMode=0;

            Qt::WindowFlags flags = 0;
            flags=Qt::CustomizeWindowHint;
            flags |= Qt::WindowStaysOnTopHint;
            //flags |=Qt::FramelessWindowHint;
            flags |=Qt::Tool;

            this->setWindowFlags(flags);
            //this->show();
            this->hide();
            this->setMaximumSize(2000,2000);
            this->setMinimumWidth(150);
            this->setMinimumHeight(50);

            this->setGeometry(this->x(),this->y(),qrectRestored.width(),qrectRestored.height());
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
        }
        else if (iMode==0)
        {
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
            qrectRestored=this->geometry();
            if (iShowUnit==1)
            {
                this->setFixedSize(150,17);
            }
            else
            {
                this->setFixedSize(100,17);
            }
            if (this->geometry().width()>99 && this->geometry().height()>15 )
            {

                lblInput->setGeometry(3,this->geometry().height()-16,12,16);
                txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
                lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
                txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);

            }


            iMode=1;

            Qt::WindowFlags flags = 0;
            flags=Qt::CustomizeWindowHint;
            flags |= Qt::WindowStaysOnTopHint;
            flags |=Qt::FramelessWindowHint;
            flags |=Qt::Tool;

            this->setWindowFlags(flags);
           // this->show();
            this->hide();
        }
    }

    if (action->text()==strMenuNetStat && bStatPoped==false && bOptPoped==false && bColorPoped==false)
    {

         qNetStat=new QGenie_qtNetStat();
         qNetStat->qtNet=this;

         Qt::WindowFlags flags = 0;
         /*
         flags=Qt::CustomizeWindowHint;
         if (this->iMode==1)
         {
             flags |=Qt::FramelessWindowHint;
         }
         flags |= Qt::Tool;
         this->setWindowFlags(flags);
         */
         //this->show();
         this->hide();

         flags=Qt::Tool;
         flags |= Qt::WindowStaysOnTopHint;
         qNetStat->setWindowFlags(flags);
         qNetStat->setWindowModality(Qt::ApplicationModal);
         bStatPoped=true;
         qNetStat->show();
    }

    if (action->text()==strMenuNetchoose && bStatPoped==false && bOptPoped==false && bColorPoped==false)
    {
         qOpt=new QGenie_qtOptions();
         initTreeWithInterfaces(qOpt->treeAdapters);
         qOpt->qtNet=this;
         InitTreeCheckState(qOpt->treeAdapters);
         //InitTreeCheckState(qOpt->comboBox);
         InitcmbCheckState();

         Qt::WindowFlags flags = 0;
         /*
         flags=Qt::CustomizeWindowHint;
         flags |= Qt::Tool;
         if (this->iMode==1)
         {
             flags |=Qt::FramelessWindowHint;
         }
         this->setWindowFlags(flags);
         */
         //this->show();
         this->hide();

         flags=Qt::Tool;
         flags |= Qt::WindowStaysOnTopHint;
         qOpt->setWindowFlags(flags);
         qOpt->setWindowModality(Qt::ApplicationModal);
         bOptPoped=true;
         qOpt->show();
    }

    if (action->text()==strMenuGraphColor && bStatPoped==false && bOptPoped==false && bColorPoped==false)
    {
        qColor=new QGenie_qtGraphColor();
        qColor->qtNet=this;

        setComboBoxCurIndex(qColor->cmbRec,qclrReceive);
        setComboBoxCurIndex(qColor->cmbSent,qclrSent);
        setComboBoxCurIndex(qColor->cmbTotal,qclrTotal);
        setComboBoxCurIndex(qColor->cmbGraphBack,qclrGraphBg);
        setComboBoxCurIndex(qColor->cmbWinBack,qclrWindowBg);

        setComboBoxCurIndex(qColor->cmbTran,dbOpt);



        Qt::WindowFlags flags = 0;
        /*
        flags=Qt::CustomizeWindowHint;
        flags |= Qt::Tool;
        if (this->iMode==1)
        {
            flags |=Qt::FramelessWindowHint;
        }
        this->setWindowFlags(flags);
        */
        //this->show();
        this->hide();

        flags=Qt::Tool;
        flags |= Qt::WindowStaysOnTopHint;
        qColor->setWindowFlags(flags);
        qColor->setWindowModality(Qt::ApplicationModal);
        bColorPoped=true;
        qColor->show();
    }


   //bFontBold
    if (action->text()==strMenuFont)
    {
        if (bFontBold==false)
        {

            txtInput->setStyleSheet("font: bold 12px;\ncolor: black;");
            txtOutput->setStyleSheet("font: bold 12px;\ncolor: black;");
            //this->setStyleSheet("font: bold 14px;\ncolor: black;");
            bFontBold=true;
        }
        else
        {
            txtInput->setStyleSheet("font: 11px;\ncolor: black;");
            txtOutput->setStyleSheet("font: 11px;\ncolor: black;");
            //this->setStyleSheet("font: 12px;\ncolor: black;");
            bFontBold=false;
        }
    }
}

void QGenie_qtNetTraffic::mouseDoubleClickEvent ( QMouseEvent * event )
{

    Genie_Log("mouseDoubleClickEvent()");
    if (event->button () ==Qt::LeftButton)
    {
        if (iMode==0)
        {
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
            qrectRestored=this->geometry();

            if (iShowUnit==1)
            {
                this->setFixedSize(150,17);
            }
            else
            {
                this->setFixedSize(100,17);
            }

            if (this->geometry().width()>99 && this->geometry().height()>15 )
            {
                lblInput->setGeometry(3,this->geometry().height()-16,12,16);
                txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
                lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
                txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            }

            iMode=1;
            Qt::WindowFlags flags = 0;
            flags=Qt::CustomizeWindowHint;
            flags |= Qt::WindowStaysOnTopHint;
            flags |=Qt::FramelessWindowHint;
            flags |=Qt::Tool;

            this->setWindowFlags(flags);
           // this->show();
            this->hide();
        }
        else
        {
            iMode=0;

            Qt::WindowFlags flags = 0;
            flags=Qt::CustomizeWindowHint;
            flags |= Qt::WindowStaysOnTopHint;
            flags |=Qt::Tool;

            this->setWindowFlags(flags);
           //
            this->hide();
           // this->show();

            this->setMaximumSize(2000,2000);
            this->setMinimumWidth(150);
            this->setMinimumHeight(50);
            this->setGeometry(this->x(),this->y(),qrectRestored.width(),qrectRestored.height());
            ptnNormal->setVisible(false);
            ptnClose->setVisible(false);
            ptnMin->setVisible(false);
        }
    }
}


void QGenie_qtNetTraffic::initialTxt()
{
    
}

void QGenie_qtNetTraffic::startGetDataThread()
{

     Genie_Log("startGetDataThread()");
     thd=new QGenieAboutThread() ;
     connect(thd,SIGNAL(sendTraffic()),this,SLOT(drawGraphic()));
     thd->mw=this;

     thd->start();
     thd->setPriority(QThread::TimeCriticalPriority);
}

void QGenie_qtNetTraffic::paintEvent ( QPaintEvent * event )
{
/*

    try
    {

        //Q_ASSERT_X(false, "divide", "division by zero");
        //QFatal("error !");
    //Genie_Log("paintEvent(QPaintEvent * event)");
    //txtInput->setText(strTxtInput);
    //txtOutput->setText(strTxtOutput);
        if (bNetOk==false)
        {

            txtInput->setVisible(false);
            txtOutput->setVisible(false);
            lblInput->setVisible(false);
            lblOutput->setVisible(false);

            lblAlert->setVisible(true);
            txtAlert->setVisible(true);

            //return;
        }
        else
        {
            txtInput->setVisible(true);
            txtOutput->setVisible(true);
            lblInput->setVisible(true);
            lblOutput->setVisible(true);
            lblAlert->setVisible(false);
            txtAlert->setVisible(false);
        }

        if (iMode==0)
        {
            this->setWindowOpacity(dbOpt);
             int iWidth=this->width();
             int iDrawHeigth=txtInput->y() -10;
             int iDrawWidth=iWidth-20;

            painter=new QPainter(this);
            painter->setPen(qclrWindowBg);
            QBrush qbrush=QBrush();

            qbrush.setColor(qclrWindowBg);
            qbrush.setStyle(Qt::SolidPattern);
            painter->setBrush(qbrush);
            painter->drawRect(0,0,this->width(),this->height());
            qbrush.setColor(qclrGraphBg);
            qbrush.setStyle(Qt::SolidPattern);
            painter->setBrush(qbrush);
            painter->drawRect(10,0,this->width()-20,iDrawHeigth);



                for (int i=TrafficEntries-1;i>=0;i--)
                {
                    if (TrafficStatsOutgoing[i].value>TrafficStatsIncoming[i].value)
                    {
                        int height;
                        if (MaxTrafficAmount<0.1)
                        {
                            continue;
                        }
                        else
                        {
                           height=iDrawHeigth*(TrafficStatsOutgoing[i].value)/MaxTrafficAmount;
                        }


                        int x=iDrawWidth-(TrafficEntries-i)*1-1+10;
                        if (x<10)
                        {
                            break;
                        }

                        int y=iDrawHeigth-height;
                        if (y<1)
                        {
                            y=1;
                        }
                        int width=1;
                        qbrush.setColor(qclrSent);
                        qbrush.setStyle(Qt::SolidPattern);
                        painter->setBrush(qbrush);
                        painter->setPen(qclrSent);
                        //painter->drawRect(x,y,width,height);
                        if (height>0)
                        {
                            painter->drawRect(x,y,width,height);
                        }

                        if (MaxTrafficAmount<0.1)
                        {
                            continue;
                        }
                        else
                        {
                           height=iDrawHeigth*(TrafficStatsIncoming[i].value)/MaxTrafficAmount;
                        }

                        x=iDrawWidth-(TrafficEntries-i)*1-1+10;
                        if (x<10)
                        {
                            break;
                        }

                        y=iDrawHeigth-height;
                        if (y<1)
                        {
                            y=1;
                        }
                        width=1;
                        qbrush.setColor(qclrReceive);
                        qbrush.setStyle(Qt::SolidPattern);
                        painter->setBrush(qbrush);
                        painter->setPen(qclrReceive);
                        //painter->drawRect(x,y,width,height);
                        if (height>0)
                        {
                            painter->drawRect(x,y,width,height);
                        }
                    }
                    else
                    {
                        int height;
                        if (MaxTrafficAmount<0.1)
                        {
                            continue;
                        }
                        else
                        {
                           height=iDrawHeigth*(TrafficStatsIncoming[i].value)/MaxTrafficAmount;
                        }

                        int x=iDrawWidth-(TrafficEntries-i)*1-1+10;
                        if (x<10)
                        {
                            break;
                        }

                        int y=iDrawHeigth-height;
                        if (y<1)
                        {
                            y=1;
                        }
                        int width=1;

                        qbrush.setColor(qclrReceive);
                        qbrush.setStyle(Qt::SolidPattern);
                        painter->setBrush(qbrush);
                        painter->setPen(qclrReceive);
                        //painter->drawRect(x,y,width,height);
                        if (height>0)
                        {
                            painter->drawRect(x,y,width,height);
                        }


                        if (MaxTrafficAmount<0.1)
                        {
                            continue;
                        }
                        else
                        {
                           height=iDrawHeigth*(TrafficStatsOutgoing[i].value)/MaxTrafficAmount;
                        }

                        x=iDrawWidth-(TrafficEntries-i)*1-1+10;
                        if (x<10)
                        {
                            break;
                        }

                        y=iDrawHeigth-height;
                        if (y<1)
                        {
                            y=1;
                        }
                        width=1;


                        qbrush.setColor(qclrSent);
                        qbrush.setStyle(Qt::SolidPattern);
                        painter->setBrush(qbrush);
                        painter->setPen(qclrSent);
                       // painter->drawRect(x,y,width,height);
                        if (height>0)
                        {
                            painter->drawRect(x,y,width,height);
                        }
                    }
                }

            //MaxTrafficAmount
            char strIn[256];
            double delta=MaxTrafficAmount / 1024.0;

            if (iunit==0)
            {
                delta=delta*8.0;
            }

            if (delta<1000.1000)
            {
                //if (iShowUnit==1)
                //{
                    if(iunit==0)
                    {
                        sprintf(strIn,"%.1fkbps",delta);
                    }
                    else
                    {
                        sprintf(strIn,"%.1fkBps",delta);
                    }

            }
            else
            {
                delta=delta/1000.0;

    //            if (iShowUnit==1)
      //          {
                    if(iunit==0)
                    {

                        sprintf(strIn,"%.2fmbps",delta);
                    }
                    else
                    {

                        sprintf(strIn,"%.2fmBps",delta);
                    }

            }

            painter->setPen(Qt::blue);
            painter->save();
            painter->translate(9, 75);
            painter->rotate(270);
            //painter->drawText(0,0, "5552.8kBps");
            painter->drawText(0,0, strIn);
            painter->restore();
            delete painter;
           // delete strIn;

        }
        else
        {

            painter=new QPainter(this);
            painter->setPen(qclrWindowBg);
            QBrush qbrush=QBrush();

            qbrush.setColor(qclrWindowBg);
            qbrush.setStyle(Qt::SolidPattern);
            painter->setBrush(qbrush);
            painter->drawRect(0,0,this->width(),this->height());
            delete painter;
        }

    }
    catch (...)
    {
        Genie_Log("QGenie_qtNetTraffic paintevent() error!");
        qFatal("QGenie_qtNetTraffic paintevent() error!" );
    }
*/
}

void QGenie_qtNetTraffic::drawGraphic()
{

    this->repaint();

}

void  QGenie_qtNetTraffic::TimerDrawtimeout ()
{

}

void QGenie_qtNetTraffic::SelectTrafficType(int trafficType)
{
      /*  switch(trafficType)
        {
        case Traffic_Incoming: 	m_cTrafficClass.SetTrafficType(QGenie_MFNetTraffic::IncomingTraffic); break;
        case Traffic_Outgoing:	m_cTrafficClass.SetTrafficType(QGenie_MFNetTraffic::OutGoingTraffic); break;
        case Traffic_Total:		m_cTrafficClass.SetTrafficType(QGenie_MFNetTraffic::AllTraffic); break;
        default:				m_cTrafficClass.SetTrafficType(QGenie_MFNetTraffic::AllTraffic);
        }
        */
}

void QGenie_qtNetTraffic::SetInterfaceNumber(int interfacenumber)
{
        SelectedInterface = interfacenumber;
        //ReInit(SelectedInterface);
}

void QGenie_qtNetTraffic::SetAdaptiveScaling(BOOL adaptive)
{
        useAdaptiveScale = adaptive;
}

void QGenie_qtNetTraffic::initTreeWithInterfaces(QTreeWidget* qtree)
{
    if (qtree)
    {
       qtree->clear();
       int iCount=m_cTrafficClass[0].Interfaces.count();
          if (iCount>0)
          {
              for (int i=0;i<iCount-1;i++)
              {
                  QString InterfaceName;
                  InterfaceName = m_cTrafficClass[0].Interfaces.value(i);
                  QTreeWidgetItem* tempItem=new QTreeWidgetItem();
                  tempItem->setText(0,InterfaceName);
                  tempItem->setCheckState(0,Qt::Checked);
                  qtree->addTopLevelItem(tempItem);
              }
              //popMenu->addMenu(actSelect);
          }
    }
}

void QGenie_qtNetTraffic::setComboBoxCurIndex(QComboBox* qcmb,QColor qclr)
{

    if (qcmb)
    {
        int r,g,b;
        QString strR,strG,strB;
        r=qclr.red();
        g=qclr.green();
        b=qclr.blue();
        char* temp=new char[15];;
        //sprintf(strIn,"%.2fMb/s",delta);

        sprintf(temp,"%d",r);
        if (r>99)
        {

            strR=QString("R")+QString(temp);
        }
        else if (r>9)
        {
            strR=QString("R0")+QString(temp);
        }
        else
        {
            strR=QString("R00")+QString(temp);
        }

        sprintf(temp,"%d",g);
        if (g>99)
        {
            strG=QString("G")+QString(temp);
        }
        else if (g>9)
        {
            strG=QString("G0")+QString(temp);
        }
        else
        {
            strG=QString("G00")+QString(temp);
        }

        sprintf(temp,"%d",b);
        if (b>99)
        {
            strB=QString("B")+QString(temp);
        }
        else if (b>9)
        {
            strB=QString("B0")+QString(temp);
        }
        else
        {
            strB=QString("B00")+QString(temp);
        }
        QString strColor=strR+QString(" ") + strG + QString(" ") + strB;
        for (int i=0;i<qcmb->count();i++)
        {
            QString strItemText=qcmb->itemText(i);

            if (strItemText.indexOf(strColor)>=0)
            {
                qcmb->setCurrentIndex(i);
                break;
            }
        }
    }
}

void QGenie_qtNetTraffic::setComboBoxCurIndex(QComboBox* qcmb,double qopt)
{

    Genie_Log("setComboBoxCurIndex()");
    if (qcmb)
    {
        //QString str=qcom->currentText();

        if ((0.99<qopt) && (qopt< 1.01))
            qcmb->setCurrentIndex(0);
         if ((0.89<qopt) && (qopt< 0.91))
            qcmb->setCurrentIndex(1);
         if ((0.79<qopt) && (qopt< 0.81))
            qcmb->setCurrentIndex(2);
         if ((0.69<qopt) && (qopt< 0.71))
            qcmb->setCurrentIndex(3);
         if ((0.59<qopt) && (qopt< 0.61))
            qcmb->setCurrentIndex(4);
         if ((0.49<qopt) && (qopt< 0.51))
            qcmb->setCurrentIndex(5);
         if ((0.39<qopt) && (qopt< 0.41))
            qcmb->setCurrentIndex(6);
         if ((0.29<qopt) && (qopt< 0.31))
            qcmb->setCurrentIndex(7);
         if ((0.19<qopt) && (qopt< 0.21))
            qcmb->setCurrentIndex(8);
    }
}


void QGenie_qtNetTraffic::iconActivated(QSystemTrayIcon::ActivationReason reason,bool toShow)
{

    /*
    Genie_Log("iconActivated()");
    switch (reason) {
    case QSystemTrayIcon::Trigger:

        if (this->isHidden() )
        {
            if (toShow==true)
            {
                Qt::WindowFlags flagsPre=this->windowFlags();

                Qt::WindowFlags flags = 0;

                flags |= Qt::WindowStaysOnTopHint;
                flags |=Qt::Tool;

                this->setWindowFlags(flags);
                //this->show();
                this->hide();


                this->setWindowFlags(flagsPre);
                //this->show();
                this->hide();
            }
        }
        else
        {
            if (toShow==false)
            {
                this->hide();
            }
        }


        if (bOptPoped==true && qOpt->isHidden()==true)
        {
            if (toShow==true)
            {
                Qt::WindowFlags flagsPre=qOpt->windowFlags();

                Qt::WindowFlags flags = 0;

                flags |= Qt::WindowStaysOnTopHint;
                flags |=Qt::Tool;

                qOpt->setWindowFlags(flags);
                qOpt->show();


                qOpt->setWindowFlags(flagsPre);
                qOpt->show();
            }
        }else if (bOptPoped==true && qOpt->isHidden()==false)
        {
            if (toShow==false)
            {
                qOpt->hide();
            }
        }


        if (bColorPoped==true && qColor->isHidden()==true)
        {
            //qColor->show();
            if (toShow==true)
            {
                Qt::WindowFlags flagsPre=qColor->windowFlags();

                Qt::WindowFlags flags = 0;

                flags |= Qt::WindowStaysOnTopHint;
                flags |=Qt::Tool;

                qColor->setWindowFlags(flags);
                qColor->show();


                qColor->setWindowFlags(flagsPre);
                qColor->show();
            }

        }else if (bColorPoped==true && qColor->isHidden()==false)
        {
            if (toShow==false)
            {
                qColor->hide();
            }
        }

        if (bStatPoped==true && qNetStat->isHidden()==true)
        {
            //qNetStat->show();
            if (toShow==true)
            {
                Qt::WindowFlags flagsPre=qNetStat->windowFlags();

                Qt::WindowFlags flags = 0;

                flags |= Qt::WindowStaysOnTopHint;
                flags |=Qt::Tool;

                qNetStat->setWindowFlags(flags);
                qNetStat->show();


                qNetStat->setWindowFlags(flagsPre);
                qNetStat->show();

            }
        }else if (bStatPoped==true && qNetStat->isHidden()==false)
        {
            if (toShow==false)
            {
                qNetStat->hide();
            }
        }
        break;
    default:
        ;
    }
    */
}

void QGenie_qtNetTraffic::messageClicked()
{
    /*
    QString strMsg;
    strMsg=QString("receiving:") + txtInput->text()+QString("\n")+QString("Sending:")+txtOutput->text();
    trayIcon->showMessage(QString(""),strMsg, QSystemTrayIcon::NoIcon,1000);
    //trayIcon->showMessage();
    */
}

void QGenie_qtNetTraffic::InitTreeCheckState(QTreeWidget* treeAdapter)
{

     Genie_Log("InitTreeCheckState()");
    if(treeAdapter)
    {

        for (int i=0;i<treeAdapter->topLevelItemCount();i++)
        {
            //int infChoose[20];
            if (i<5)
            {
                treeAdapter->topLevelItem(i)->setCheckState(0,Qt::Unchecked);
            }
        }

        for (int i=0;i<treeAdapter->topLevelItemCount();i++)
        {
            //int infChoose[20];
            if (i<3)
            {
                if(infChoose[i]==1)
                {
                    treeAdapter->topLevelItem(i)->setCheckState(0,Qt::Checked);
                }
            }
        }
    }
}

void QGenie_qtNetTraffic::InitTreeCheckState(QComboBox* cmbUnit)
{

}

void QGenie_qtNetTraffic::InitcmbCheckState()
{


    Genie_Log("InitcmbCheckState()");
    if (iShowUnit==1)
    {
        qOpt->rdbShowUnit->setChecked(true);
    }
    else
    {
        qOpt->rdbNotShow->setChecked(true);
    }

    if (iunit==0)
    {
        qOpt->rdbkb->setChecked(true);
    }
    else
    {
        qOpt->rdbKB->setChecked(true);
    }

}

void	QGenie_qtNetTraffic::showEvent ( QShowEvent * event )
{
   // Genie_Log("showEvent(QShowEvent * event)");
    if (iMode==1)
    {
        if (iShowUnit==1)
        {
            this->setFixedSize(150,17);

            lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
        }
        else
        {
            this->setFixedSize(100,17);

            lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);

        }

    }
    this->repaint();
}

