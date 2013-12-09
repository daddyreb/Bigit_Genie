#include "QGenie_qtNetTraffic.h"
//#include "QGenie_qtNetStat.h"
//#include "QGenie_qtGraphColor.h"
//#include "QGenie_qtOptions.h"
#include <QtGui/QTreeWidgetItem>
#include <QBrush>
#include <QString>
#include <QFont>
#include <QtGui/QScreen>
#include <QDesktopWidget>
#include <QRect>
#include "QGenieAboutThread.h"
#include "ISettings.h"
#include "setting_def.h"

#include "Genieplugin_Internet.h"

//#include "IInfoManager.h"

////#include "IUiManager.h"
//#include "IPluginInf.h"
//#include "ILanguageManager.h"
//#include "LanguageNetTraffic.h"
#include <QEvent>

#include <QUuid>
#include "IUiManager.h"

//IManagerHandler *QGenie_qtNetTraffic::sHandler=NULL;
extern ISettings *g_settings;

QGenie_qtNetTraffic::QGenie_qtNetTraffic(QWidget *parent)
    : QWidget(parent)
{
    //this->setWindowIcon(QIcon(":/image/aa.ico"));


   // IInfoManager* Iinfo=sHandler->getInfoManager();
    //QRect rect=Iinfo->getGeometry(INFO_CMD_MAIN_GEOMETRY);

   // this->setGeometry(rect.x()+rect.width()+20,rect.y()+rect.height()-20,150,130);

    //this->setMinimumWidth(150);
    //this->setMinimumHeight(50);
    iMode=0;
    /*
    QFont qft=QFont();

    qft.setFamily(QString("MS Sans Serif"));
    qft.setBold(true);
    this->setFont(qft);

    */

    //font: 75 16pt "宋体";
    this->setStyleSheet("font: 75 12pt;\ncolor:black;");
    bFontBold=false;
    bDrawOrNot=true;
    qrectRestored=this->geometry();
    bNull=true;
    bNetOk=true;
    bDrawInput=true;
    bDrawOutput=true;
    bDrawTotal=true;
/*
    Qt::WindowFlags flags = 0;
    flags=Qt::CustomizeWindowHint;
    flags |= Qt::WindowStaysOnTopHint;
    //flags |=Qt::FramelessWindowHint;
    flags |=Qt::Tool;
    this->setWindowFlags(flags);
*/

    lblInput=new QLabel(this);
    lblInput->setPixmap(QPixmap(":/image/download_big.png"));
    lblInput->setGeometry(3,114,16,16);
    //lblInput->setToolTip(QString("Download"));

    txtInput=new   QLabel(this);

    txtInput->setGeometry(19,114,127,16);
    txtInput->setTextFormat(Qt::PlainText);
    txtInput->setStyleSheet("font: 14px;");


    lblOutput=new QLabel(this);
    lblOutput->setPixmap(QPixmap(":/image/upload_big.png"));
    lblOutput->setGeometry(83,114,16,16);
   // lblOutput->setToolTip(QString("Upload"));

    txtOutput=new   QLabel(this);


    txtOutput->setGeometry(99,114,127,16);
    txtOutput->setTextFormat(Qt::PlainText);
    txtOutput->setStyleSheet("font: 14px;");


    horizontalSlider = new QSlider(this);

    horizontalSlider->setGeometry(QRect(209, 175, 71, 21));
    horizontalSlider->setOrientation(Qt::Horizontal);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(20);
    horizontalSlider->setFocusPolicy(Qt::NoFocus);

    //valueChanged()
    connect(horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(slideValueChanged(int)));


    checkBox= new QCheckBox(this);
    checkBox->setGeometry(QRect(20, 177, 211, 16));


    lblMb= new QLabel(this);
    lblMb->setObjectName(QString::fromUtf8("lblMb"));
    lblMb->setGeometry(QRect(309, 175, 71, 21));
    lblMb->setText("50kbps");
    lblMb->setStyleSheet("font: 14px;\ncolor:black;");


    checkBoxInput= new QCheckBox(this);
    checkBoxInput->setText("");

    // painter->drawRect(50,20, 10,10);


    /*checkBox->setGeometry(QRect(20, 177, 111, 16));*/

    checkBoxOutput= new QCheckBox(this);
    checkBoxOutput->setText("");
    checkBoxTotal= new QCheckBox(this);
    checkBoxTotal->setText("");

    checkBoxInput->setGeometry(QRect(30, 18, 20, 16));
    checkBoxOutput->setGeometry(QRect(30, 38, 20, 16));
    checkBoxTotal->setGeometry(QRect(30, 58, 20, 16));

    checkBox->setFocusPolicy(Qt::NoFocus);
    checkBoxInput->setFocusPolicy(Qt::NoFocus);
    checkBoxOutput->setFocusPolicy(Qt::NoFocus);
    checkBoxTotal->setFocusPolicy(Qt::NoFocus);

    checkBoxInput->setChecked(true);
    checkBoxOutput->setChecked(true);
    checkBoxTotal->setChecked(false);
    checkBoxTotal->setVisible(false);

    connect(checkBoxInput,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChangedInput(int)));
    connect(checkBoxOutput,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChangedOutput(int)));
    connect(checkBoxTotal,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChangedTotal(int)));




    if (GET_SETTING_BOOL (PLUGIN_TRAFFIC_WARRING) )
    {
        checkBox->setChecked(true);
        iAlert=GET_SETTING_INT (PLUGIN_TRAFFIC_THRESHOLD) ;

        lblMb->setText(QString::number(iAlert) + "Mbps");
        //horizontalSlider->setValue(iAlert/100);
        horizontalSlider->setValue(iAlert);
        lblMb->setStyleSheet("font: 14px;\ncolor:black;");

    }
    else
    {
        checkBox->setChecked(false);
         lblMb->setStyleSheet("font: 14px;\ncolor:gray;");
        //iAlert=50
    }




    checkBox->setFocusPolicy(Qt::NoFocus);
    checkBox->setStyleSheet("font: 14px;\ncolor:black;");
    //void QCheckBox::stateChanged ( int state )
    connect(checkBox,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChanged(int)));





    bAlert=true;


    Timerdraw=new QTimer();
    Timerdraw->setInterval(1000);
    Timerdraw->start();
    connect(Timerdraw,SIGNAL(timeout ()),this,SLOT(TimerDrawtimeout ()));
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
    iDrawCount=this->width()-40;
    //TrafficEntries = TGSize.cx / plotgranularity;
   // TrafficStats = new TRAFFICENTRY[TrafficEntries+1];
   // TrafficStatsIncoming=new TRAFFICENTRY[TrafficEntries+1];
   // TrafficStatsOutgoing=new TRAFFICENTRY[TrafficEntries+1];

    for(DWORD x=0; x<TrafficEntries; x++)
    {
        /*
            TrafficStats[x].connected = TRUE;
            TrafficStats[x].value	= 0.0;
            TrafficStatsIncoming[x].connected = TRUE;
            TrafficStatsIncoming[x].value	= 0.0;
            TrafficStatsOutgoing[x].connected = TRUE;
            TrafficStatsOutgoing[x].value	= 0.0;
        */
    }

    qclrReceive=QColor(0,113,215);
    qclrSent=QColor(0,153,0);
    qclrTotal=QColor(255,204,0);
    //qclrWindowBg=QColor(222,255,255);
    qclrWindowBg=QColor(242,242,242);
    qclrGraphBg=QColor(235,235,235);
    qclrRectLine=QColor(178,168,168);
    //qclrWindowBg=QColor(153,204,255);


    dbOpt=1;
    this->setWindowOpacity(dbOpt);

    iunit=0;
    iShowUnit=1;
    bFontBold=false;
    for (int i=0;i<3;i++)
    {
        infChoose[i]=1;
    }
/*
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

*/


    txtInput->setAlignment(Qt::AlignCenter);
    txtOutput->setAlignment(Qt::AlignCenter);

    //leftkeypressed=0;
    //createTrayIcon();

    //retranslateUi();

    //connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
   // connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          //  this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

   // trayIcon->show();

    bOptPoped=false;
    bColorPoped=false;
    bStatPoped=false;

   // minClicked(false);  need translate;


    retranslateUi();

    this->setMinimumWidth(520);
    this->setMinimumHeight(150);


   // painter=new QPainter(this);
}

 void	QGenie_qtNetTraffic::showEvent ( QShowEvent * event )
{
    bDrawOrNot=true;
}

 void	QGenie_qtNetTraffic::hideEvent ( QHideEvent * event )
{
    bDrawOrNot=false;
}

void QGenie_qtNetTraffic::sendTrafficAlertSlot(bool bAlert)
{
    emit  TrafficAlert( bAlert);
}


void QGenie_qtNetTraffic::checkBoxStateChanged(int checkState)
{
    if (checkState==Qt::Checked)
    {
        horizontalSlider->setEnabled(true);
        lblMb->setEnabled(true);
        iAlert=horizontalSlider->value();
        lblMb->setStyleSheet("font: 14px;\ncolor:black;");
        bAlert=true;
    }
    else
    {
        horizontalSlider->setEnabled(false);
        lblMb->setEnabled(false);
        bAlert=false;
        lblMb->setStyleSheet("font: 14px;\ncolor:gray;");
        emit  TrafficAlert( false);
    }
}

void QGenie_qtNetTraffic::checkBoxStateChangedInput(int checkState)
{
    if (checkState==Qt::Checked)
    {
        bDrawInput=true;

    }
    else
    {
        bDrawInput=false;

    }
}

void QGenie_qtNetTraffic::checkBoxStateChangedOutput(int checkState)
{
    if (checkState==Qt::Checked)
    {

        bDrawOutput=true;

    }
    else
    {

        bDrawOutput=false;

    }
}

void QGenie_qtNetTraffic::checkBoxStateChangedTotal(int checkState)
{
    if (checkState==Qt::Checked)
    {
        bDrawTotal=true;
    }
    else
    {
        bDrawTotal=false;
    }
}


void QGenie_qtNetTraffic::slideValueChanged(int value)
{
    iAlert=value;
    bAlert=true;
    lblMb->setText(QString::number(value)+ "Mbps");
}

void QGenie_qtNetTraffic::retranslateUi()
{

    strUploadSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_UPLOAD_LB);
    strDownloadSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_DOWNLOAD_LB);
    strInputSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_DOWNLOAD);
    strOutputSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_UPLOAD);
    strTotalSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_TOTALLOAD);

    checkBox->setText(GeniePlugin_Internet::translateText(L_INTERNET_TRAFFIC_ALERTSETTING_CB_TEXT));

   // checkBox->setText("Alerg Setting");

}


void QGenie_qtNetTraffic::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}


QGenie_qtNetTraffic::~QGenie_qtNetTraffic()
{
    //horizontalSlider
    if (checkBox)
    {
        if (checkBox->isChecked())
        {
            SET_SETTING_VALUE(PLUGIN_TRAFFIC_WARRING,true) ;
            SET_SETTING_VALUE(PLUGIN_TRAFFIC_THRESHOLD,iAlert);
        }
        else
        {
            SET_SETTING_VALUE(PLUGIN_TRAFFIC_WARRING,false) ;
            SET_SETTING_VALUE(PLUGIN_TRAFFIC_THRESHOLD,iAlert);
        }
    }

    if (Timerdraw)
    {
        Timerdraw->deleteLater();
        //delete Timerdraw;
    }
}

void QGenie_qtNetTraffic::startGetDataThread()
{
     thd=new QGenieAboutThread(this) ;
     connect(thd,SIGNAL(sendTraffic()),this,SLOT(drawGraphic()));
     connect(thd,SIGNAL(sendTrafficAlert(bool)),this,SLOT(sendTrafficAlertSlot(bool)));
     thd->mw=this;
     thd->start();
}

void QGenie_qtNetTraffic::drawGraphic()
{

    this->repaint();

}

/*
void QGenie_qtNetTraffic::createTrayIcon()
{

    //qmenuTroy
    qmenuTroy=new QMenu(QString("Net Traffic"),this);
    qmenuTroy->setIcon(QIcon(":/image/Net Traffic.png"));

    actTrayNormal=new QAction("Switch Mode",0);
    qmenuTroy->addAction(actTrayNormal);


    qmenuTroy->addSeparator();


    actTrayFont=new QAction("Switch Font",0);
    qmenuTroy->addAction(actTrayFont);

    qmenuTroy->addSeparator();

    actTrayNetchoose=new QAction("General Options",0);
    qmenuTroy->addAction(actTrayNetchoose);

    actTrayGraphColor=new QAction("Graph Color",0);
    qmenuTroy->addAction(actTrayGraphColor);

    actTrayNetStat=new QAction("NetStat List",0);
    qmenuTroy->addAction(actTrayNetStat);

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
        this->show();
    }
}

void QGenie_qtNetTraffic::closeClicked(bool checked)
{
    this->hide();
}

void QGenie_qtNetTraffic::normalClicked(bool checked)
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
        this->show();

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

 */
void	QGenie_qtNetTraffic::resizeEvent ( QResizeEvent * event )
{

    /*

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



    }
    else
    {
        ptnNormal->setVisible(false);
        ptnClose->setVisible(false);
        ptnMin->setVisible(false);
    }
*/

   // qrectRestored=this->geometry(); this

    if (this->geometry().width()>519 && this->geometry().height()>15 )
    {
        /*
            QSlider *horizontalSlider;
            QCheckBox *checkBox;
            QLabel *lblMb;
        */

        int iWidth=this->width();
        checkBox->setGeometry(150,this->geometry().height()-23,250,20);
        horizontalSlider->setGeometry(20,this->geometry().height()-20,60,16);
        lblMb->setGeometry(85,this->geometry().height()-22,60,18);

        lblInput->setGeometry(iWidth*4/5 -105,this->geometry().height()-20,16,16);
        txtInput->setGeometry(iWidth*4/5-85,this->geometry().height()-20,86,16);
        lblOutput->setGeometry(iWidth*4/5+10,this->geometry().height()-20,16,16);
        txtOutput->setGeometry(iWidth*4/5+30,this->geometry().height()-20,86,16);

        //checkBoxInput->setGeometry(QRect(30, 18, 20, 16));
        //checkBoxOutput->setGeometry(QRect(30, 38, 20, 16));
        //checkBoxTotal->setGeometry(QRect(30, 58, 20, 16));

        checkBoxInput->setGeometry(QRect(30,8, 16, 16));
        checkBoxOutput->setGeometry(QRect(180, 8, 16, 16));
        //checkBoxTotal->setGeometry(QRect(30, 58, 20, 16));
    }
    iDrawCount=this->width()-40;
}

/*
void QGenie_qtNetTraffic::mousePressEvent ( QMouseEvent * event )
{
    //生成菜单
    if (event->button () ==Qt::RightButton)
    {



        QMenu* popMenu=new QMenu(this);

        QAction* actNormal;
        QAction* actFont;
        QAction* actNetchoose;
        QAction* actGraphColor;
        QAction* actNetStat;



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
        popMenu->addAction(actGraphColor);
        popMenu->addAction(actNetStat);

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
            this->show();

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
            this->show();
        }
    }

    if (action->text()==strMenuNetStat && bStatPoped==false && bOptPoped==false && bColorPoped==false)
    {

         qNetStat=new QGenie_qtNetStat();
         qNetStat->qtNet=this;
         Qt::WindowFlags flags = 0;
         flags=Qt::CustomizeWindowHint;
         if (this->iMode==1)
         {
             flags |=Qt::FramelessWindowHint;
         }
         flags |= Qt::Tool;
         this->setWindowFlags(flags);
         this->show();

         flags=Qt::Tool;
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

         Qt::WindowFlags flags = 0;
         flags=Qt::CustomizeWindowHint;
         flags |= Qt::Tool;
         if (this->iMode==1)
         {
             flags |=Qt::FramelessWindowHint;
         }
         this->setWindowFlags(flags);
         this->show();

         flags=Qt::Tool;
         qOpt->setWindowFlags(flags);

         InitTreeCheckState(qOpt->treeAdapters);
         //InitTreeCheckState(qOpt->comboBox);
         InitcmbCheckState();
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
        flags=Qt::CustomizeWindowHint;
        flags |= Qt::Tool;
        if (this->iMode==1)
        {
            flags |=Qt::FramelessWindowHint;
        }
        this->setWindowFlags(flags);
        this->show();

        flags=Qt::Tool;
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
            this->show();
        }
        else
        {
            iMode=0;

            Qt::WindowFlags flags = 0;
            flags=Qt::CustomizeWindowHint;
            flags |= Qt::WindowStaysOnTopHint;
            flags |=Qt::Tool;

            this->setWindowFlags(flags);
            this->show();

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
*/

void QGenie_qtNetTraffic::paintEvent(QPaintEvent *)
{
   // strUploadSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_UPLOAD_LB);
   // strDownloadSpd=GeniePlugin_Internet::translateText(L_NETTRAFFIC_DOWNLOAD_LB);

   // txtInput->setText(strUploadSpd + " " + strTxtInput);
    //txtInput->setText(strTxtInput);
    //txtOutput->setText(strDownloadSpd + " " +strTxtOutput);
    //txtOutput->setText(strTxtOutput);

    QString strouttemp;
    if (bNull)
        return;
    if (iMode==0)
    {
        this->setWindowOpacity(dbOpt);
        int iWidth=this->width();
        int iDrawHeigth=txtInput->y() -30;
        int iDrawWidth=iWidth-40;

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
        painter->drawRect(20,0,this->width()-40,iDrawHeigth+20);

        qbrush.setStyle(Qt::NoBrush);
        //qbrush.setColor(qclrRectLine);
        //qbrush.setColor(Qt::red);
        painter->setBrush(qbrush);
        painter->setPen(qclrRectLine);
        painter->drawRect(19,1,this->width()-38,iDrawHeigth+20);

        /*
        qbrush.setColor(qclrTotal);
        qbrush.setStyle(Qt::SolidPattern);
        painter->setBrush(qbrush);
        painter->setPen(qclrTotal);
        if (bDrawTotal==true)
        {
            for (int i=TrafficEntries-1;i>=0;i--)
            {
                int height;
                if (MaxTrafficAmount<0.1)
                {
                    continue;
                }
                else
                {
                   height=iDrawHeigth*(TrafficStats[i].value)/MaxTrafficAmount;
                }

                int x=iDrawWidth-(TrafficEntries-i)*1-1+20;
                if (x<20)
                {
                    break;
                }

                int y=iDrawHeigth-height;
                int width=1;
                if (height>0)
                {
                    painter->drawRect(x,y+21,width,height);
                }

            }
        }
    */


        for (int i=TrafficEntries;i>=0;i--)
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
                   if (TrafficStatsOutgoing[i].value<0.01*1000*1024/8.0)
                       height=0;
                   if (height>iDrawHeigth)
                       height=iDrawHeigth;
                }


                int x=iDrawWidth-(TrafficEntries-i)*1-1+20;
                if (x<20)
                {
                    break;
                }

                int y=iDrawHeigth-height;
                int width=1;
                qbrush.setColor(qclrSent);
                qbrush.setStyle(Qt::SolidPattern);
                painter->setBrush(qbrush);
                painter->setPen(qclrSent);
                if (height>0)
                {
                    if (bDrawOutput)
                    {
                        painter->drawRect(x,y+21,width,height);
                    }
                }

                if (MaxTrafficAmount<0.1)
                {
                    continue;
                }
                else
                {
                   height=iDrawHeigth*(TrafficStatsIncoming[i].value)/MaxTrafficAmount;
                   if (TrafficStatsIncoming[i].value<0.01*1000*1024/8.0)
                       height=0;
                   if (height>iDrawHeigth)
                       height=iDrawHeigth;
                }

                x=iDrawWidth-(TrafficEntries-i)*1-1+20;
                if (x<20)
                {
                    break;
                }

                y=iDrawHeigth-height;
                width=1;
                qbrush.setColor(qclrReceive);
                qbrush.setStyle(Qt::SolidPattern);
                painter->setBrush(qbrush);
                painter->setPen(qclrReceive);
                if (height>0)
                {
                    if (bDrawInput)
                    {
                        painter->drawRect(x,y+21,width,height);
                    }
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
                   if (TrafficStatsIncoming[i].value<0.01*1000*1024/8.0)
                       height=0;
                   if (height>iDrawHeigth)
                       height=iDrawHeigth;
                }

                int x=iDrawWidth-(TrafficEntries-i)*1-1+20;
                if (x<20)
                {
                    break;
                }

                int y=iDrawHeigth-height;
                int width=1;

                qbrush.setColor(qclrReceive);
                qbrush.setStyle(Qt::SolidPattern);
                painter->setBrush(qbrush);
                painter->setPen(qclrReceive);
                if (height>0)
                {
                    if (bDrawInput)
                    {
                        painter->drawRect(x,y+21,width,height);
                    }
                }


                if (MaxTrafficAmount<0.1)
                {
                    continue;
                }
                else
                {
                   height=iDrawHeigth*(TrafficStatsOutgoing[i].value)/MaxTrafficAmount;
                   if (TrafficStatsOutgoing[i].value<0.01*1000*1024/8.0)
                       height=0;
                   if (height>iDrawHeigth)
                       height=iDrawHeigth;
                }

                x=iDrawWidth-(TrafficEntries-i)*1-1+20;
                if (x<20)
                {
                    break;
                }


                y=iDrawHeigth-height;
                width=1;


                qbrush.setColor(qclrSent);
                qbrush.setStyle(Qt::SolidPattern);
                painter->setBrush(qbrush);
                painter->setPen(qclrSent);
                if (height>0)
                {
                    if (bDrawOutput)
                    {
                        painter->drawRect(x,y+21,width,height);
                    }
                }
            }
        }

        //MaxTrafficAmount
        char strIn[256];
        double delta=MaxTrafficAmount / 1024.0;

        if (iunit==0)
        {
            delta=delta*8;
            /*
            if (delta<1000.0000)
            {
                sprintf(strIn,"%.1fkbps",delta);
            }
            else
            {
            */
                delta=delta/1000.0;


               // sprintf(strIn,"%.2fMbps",delta);

                sprintf(strIn,"%.2f",delta);
                strouttemp=strIn;
                if (strouttemp.right(1)=="0")
                    strouttemp=strouttemp.left(strouttemp.length()-1);

                if (strouttemp.right(1)=="0")
                    strouttemp=strouttemp.left(strouttemp.length()-1);

                if (strouttemp.right(1)==".")
                    strouttemp=strouttemp.left(strouttemp.length()-1);

                strouttemp=strouttemp+"Mbps";

           // }
        }
        else
        {
            if (delta<1000.0000)
            {
                sprintf(strIn,"%.1fkBps",delta);
            }
            else
            {
                delta=delta/1000.0;
                sprintf(strIn,"%.2fMBps",delta);
            }
        }


        //在此画3个说明的图

        //strInputSpd="DownLoad";
        //strOutputSpd="Upload";
        //strTotalSpd="Total";

        //qclrReceive=QColor(255,51,0);

        QBrush qbrushA=QBrush();
        //pen.setColor();
        painter->setPen(qclrReceive);
        painter->save();
        //painter->drawText(62,30,strInputSpd );
        painter->drawText(62,20,strInputSpd );

        qbrushA.setColor(qclrReceive);
        qbrushA.setStyle(Qt::SolidPattern);
        painter->setBrush(qbrushA);
        //painter->drawRect(50,20, 10,10);
        painter->drawRect(50,10, 10,10);

        painter->restore();


        //qclrSent=QColor(0,153,0);
        painter->setPen(qclrSent);
        painter->save();

        //painter->drawText(62,50,strOutputSpd );

        painter->drawText(212,20,strOutputSpd );

        qbrushA.setColor(qclrSent);
        qbrushA.setStyle(Qt::SolidPattern);
        painter->setBrush(qbrushA);
        //painter->drawRect(50,40, 10,10);

        painter->drawRect(200,10, 10,10);
        painter->restore();

        //qclrTotal=QColor(255,204,0);


        /*
        painter->setPen(qclrTotal);
        painter->save();

        painter->drawText(62,70,strTotalSpd );



        qbrushA.setColor(qclrTotal);
        qbrushA.setStyle(Qt::SolidPattern);
        painter->setBrush(qbrushA);
        painter->drawRect(50,60, 10,10);

        painter->restore();
*/

        painter->setPen(Qt::black);
        painter->save();
        painter->translate(16, 90);
        painter->rotate(270);
        //painter->drawText(0,0, "552.8kbps");
        painter->drawText(0,0, strouttemp);


        painter->restore();
        delete painter;
        //delete strIn;
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

void  QGenie_qtNetTraffic::TimerDrawtimeout ()
{
   //    AllTraffic		= 388,//总的流量
   //  IncomingTraffic	= 264,//输入流量
   // OutGoingTraffic	= 506 //输出流量

/*

    int iCount=m_cTrafficClass1[0].Interfaces.count();

    double trafficIncomingSum=0.0;
    for (int i=0;i<iCount;i++)
    {
        if (i<5)
        {
             m_cTrafficClass1[i].SetTrafficType(QGenie_MFNetTraffic::IncomingTraffic);
            if (infChoose[i]==1)
            {
                double traffic1 = m_cTrafficClass1[i].GetTraffic(i);
                trafficIncomingSum=trafficIncomingSum+traffic1;
            }
        }
    }

    char* strIn=new char[15];
    double delta1;
    double divisor = (1000.0/(double)NETUPDATESPEED);


    if ((trafficIncomingSum/1024.0)<1000.0000)
    {

        if (iShowUnit==1 || iMode==0)
        {
            if (iunit==0)
            {
                delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0;
                sprintf(strIn,"%.1fkbps",delta1);
            }
            else
            {
                delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1fkbps",delta1);
            }
        }
        else
        {
            if (iunit==0)
            {
                delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0;
                sprintf(strIn,"%.1f",delta1);
            }
            else
            {
                delta1 = (double)(trafficIncomingSum * divisor) / 1024.0;
                sprintf(strIn,"%.1f",delta1);
            }
        }
    }
    else
    {
        if (iShowUnit==1 || iMode==0)
        {
            if (iunit==0)
            {
                delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0/1000.0;
                sprintf(strIn,"%.2fmbps",delta1);
            }
            else
            {
                delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2fmBps",delta1);
            }
        }
        else
        {
            if (iunit==0)
            {
                delta1 = (double)(trafficIncomingSum * divisor)*8.0 / 1024.0/1000.0;
                sprintf(strIn,"%.2f",delta1);
            }
            else
            {
                delta1 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strIn,"%.2f",delta1);
            }
        }
    }


    CurrentTraffic=strIn;
    txtInput->setText(strDownloadSpd + CurrentTraffic);


    double trafficOutgoingSum=0.0;
    for (int i=0;i<iCount;i++)
    {
        if (i<5)
        {
            m_cTrafficClass2[i].SetTrafficType(QGenie_MFNetTraffic::OutGoingTraffic);
            if (infChoose[i]==1)
            {
                double traffic1 = m_cTrafficClass2[i].GetTraffic(i);
                trafficOutgoingSum=trafficOutgoingSum+traffic1;
            }
        }
    }

    char* strOut=new char[15];

    double delta2;


    if ((trafficOutgoingSum/1024.0)<1000.2)
    {
        if (iShowUnit==1 || iMode==0)
        {
            if (iunit==0)
            {
                delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0;
                sprintf(strOut,"%.1fkbps",delta2);
            }
            else
            {
                delta2 = (double)(trafficOutgoingSum * divisor)/ 1024.0;
                sprintf(strOut,"%.1fkbps",delta2);
            }
        }
        else
        {

            if (iunit==0)
            {
                delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0;
                sprintf(strOut,"%.1f",delta2);
            }
            else
            {
                delta2 = (double)(trafficOutgoingSum * divisor)/ 1024.0;
                sprintf(strOut,"%.1f",delta2);
            }
        }
    }
    else
    {
        if (iShowUnit==1 || iMode==0)
        {
            if (iunit==0)
            {
                delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0/1000.0;
                sprintf(strOut,"%.2fmbps",delta2);
            }
            else
            {
                delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2fmbps",delta2);
            }
        }
        else
        {
            if (iunit==0)
            {
                delta2 = (double)(trafficOutgoingSum * divisor)*8.0 / 1024.0/1000.0;
                sprintf(strOut,"%.2f",delta2);
            }
            else
            {
                delta2 = (double)(trafficIncomingSum * divisor) / 1024.0/1000.0;
                sprintf(strOut,"%.2f",delta2);
            }
        }
    }

    CurrentTraffic=strOut;
    txtOutput->setText(strUploadSpd+CurrentTraffic);


    double trafficAlltrafficSum=0.0;
    for (int i=0;i<iCount;i++)
    {
        if (i<5)
        {
             m_cTrafficClass[i].SetTrafficType(QGenie_MFNetTraffic::AllTraffic);
            if (infChoose[i]==1)
            {
                double traffic1 = m_cTrafficClass[i].GetTraffic(i);
                trafficAlltrafficSum=trafficAlltrafficSum+traffic1;
            }
        }
    }


    if(useAdaptiveScale==TRUE)
    {

    }


    for(DWORD x=0; x<TrafficEntries; x++)
    {
            TrafficStats[x].connected = TrafficStats[x+1].connected;
            TrafficStats[x].value	= TrafficStats[x+1].value;


            TrafficStatsIncoming[x].connected = TrafficStatsIncoming[x+1].connected;
            TrafficStatsIncoming[x].value	= TrafficStatsIncoming[x+1].value;


            TrafficStatsOutgoing[x].connected = TrafficStatsOutgoing[x+1].connected;
            TrafficStatsOutgoing[x].value	= TrafficStatsOutgoing[x+1].value;

    }

    TrafficStats[TrafficEntries].connected = TRUE;
    TrafficStats[TrafficEntries].value = trafficAlltrafficSum;


    TrafficStatsIncoming[TrafficEntries].connected = TRUE;
    TrafficStatsIncoming[TrafficEntries].value = trafficIncomingSum;


    TrafficStatsOutgoing[TrafficEntries].connected = TRUE;
    TrafficStatsOutgoing[TrafficEntries].value = trafficOutgoingSum;

    MaxTrafficAmount = 0.0;
    for(DWORD x=TrafficEntries-iDrawCount; x<TrafficEntries; x++)
    {
        if(TrafficStats[x].value > MaxTrafficAmount)
                MaxTrafficAmount = TrafficStats[x].value;

        if(TrafficStatsIncoming[x].value > MaxTrafficAmount)
                MaxTrafficAmount = TrafficStatsIncoming[x].value;

        if(TrafficStatsOutgoing[x].value > MaxTrafficAmount)
                MaxTrafficAmount = TrafficStatsOutgoing[x].value;
    }


    double delta4;
    divisor = (1000.0/(double)NETUPDATESPEED);

    if (iunit==0)
    {
        delta4 = (double)(MaxTrafficAmount * divisor)*8.0/ 1024.0;
    }
    else
    {
        delta4 = (double)(MaxTrafficAmount * divisor) / 1024.0 ;
    }
    this->repaint();

*/
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

/*
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
                this->show();


                this->setWindowFlags(flagsPre);
                this->show();
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
}

*/

/*
void QGenie_qtNetTraffic::messageClicked()
{

}

void QGenie_qtNetTraffic::InitTreeCheckState(QTreeWidget* treeAdapter)
{
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
            if (i<5)
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
*/
