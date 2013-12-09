#include "QGenie_qtOptions.h"
#include <QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>
#include "plugin_ui_def.h"
#include "QGenie_qtNetTraffic.h"
#include "ILanguageManager.h"
#include "LanguageNetTraffic.h"
#include <QEvent>

QGenie_qtOptions::QGenie_qtOptions(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    //this->setWindowTitle("Options");
    this->setWindowIcon(QIcon(":/image/aa.ico"));

   // Qt::WindowFlags flags = 0;
    //flags=Qt::CustomizeWindowHint;
   // flags |= Qt::WindowStaysOnTopHint;
    //flags |=Qt::FramelessWindowHint;
   // flags |=Qt::Tool;
   // this->setWindowFlags(flags);

    SET_FIX_SIZE(NORMAL_BUTTON,btnOk);
    //btnOk->setFocusPolicy(Qt::NoFocus);

    SET_FIX_SIZE(NORMAL_BUTTON,btnCancel);
    //btnCancel->setFocusPolicy(Qt::NoFocus);

}

void QGenie_qtOptions::setupUi(QWidget *Widget)
{
    if (Widget->objectName().isEmpty())
        Widget->setObjectName(QString::fromUtf8("Widget"));
    Widget->resize(418, 416);
    Widget->setGeometry(350,200,418,416);
    Widget->setWindowFlags(Qt::WindowCloseButtonHint);
    frame = new QFrame(Widget);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setGeometry(QRect(0, 10, 411, 361));

    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
   // lblGeneral = new QLabel(frame);
  //  lblGeneral->setObjectName(QString::fromUtf8("lblGeneral"));
   // lblGeneral->setGeometry(QRect(10, 1, 121, 16));



    gboxUnits = new QGroupBox(frame);
    gboxUnits->setGeometry(10,5,391,60);

    rdbkb=new QRadioButton(gboxUnits);
    rdbkb->setGeometry(10,36,360,22);
    rdbKB=new QRadioButton(gboxUnits);
    rdbKB->setGeometry(10,12,360,22);
    rdbKB->setChecked(true);
    //comboBox = new QComboBox(gboxUnits);

    gboxShow=new QGroupBox(frame);
    gboxShow->setGeometry(10,80,391,60);

    rdbShowUnit=new QRadioButton(gboxShow);
    rdbShowUnit->setGeometry(10,12,360,22);
    rdbNotShow=new QRadioButton(gboxShow);
    rdbNotShow->setGeometry(10,36,360,22);
    rdbShowUnit->setChecked(true);

    gboxAdapters = new QGroupBox(frame);
    gboxAdapters->setObjectName(QString::fromUtf8("gboxAdapters"));
    gboxAdapters->setGeometry(QRect(10, 160, 391, 211));
    lblAdapter = new QLabel(gboxAdapters);
    lblAdapter->setObjectName(QString::fromUtf8("lblAdapter"));
    lblAdapter->setGeometry(QRect(10, 20, 371, 21));
    treeAdapters = new QTreeWidget(gboxAdapters);
    QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
    __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
    treeAdapters->setHeaderItem(__qtreewidgetitem);
    treeAdapters->setObjectName(QString::fromUtf8("treeAdapters"));
    treeAdapters->setGeometry(QRect(10, 50, 371, 151));
    treeAdapters->header()->setVisible(false);
    treeAdapters->setRootIsDecorated(false);
    treeAdapters->setAlternatingRowColors(true);

    btnOk = new QPushButton(Widget);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setGeometry(QRect(240, 380, 75, 23));
    btnCancel = new QPushButton(Widget);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setGeometry(QRect(330, 380, 75, 23));


    connect(btnOk,SIGNAL(clicked ( bool  )),this,SLOT(okClicked (bool)));
    connect(btnCancel,SIGNAL(clicked ( bool  )),this,SLOT(cancelClicked (bool)));

    retranslateUi(Widget);

    QMetaObject::connectSlotsByName(Widget);
} // setupUi


void QGenie_qtOptions::retranslateUi(QWidget *Widget)
{
    //Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0, QApplication::UnicodeUTF8));

   // ILanguageManager *lanmanager=qtNet->sHandler->getLanguageManager();
    //lanmanager->getText(L_NETTRAFFIC_PROCTO)
    /*
        L_NETTRAFFIC_DATA_UNITS,
        L_NETTRAFFIC_BITS,
        L_NETTRAFFIC_BYTES,
        L_NETTRAFFIC_SHOW_UNIT_OR_NOT,
        L_NETTRAFFIC_SHOW_UNIT,
        L_NETTRAFFIC_HIDE_UNIT,
        L_NETTRAFFIC_NETWORK_ADAPTERS,
        L_NETTRAFFIC_WHICH_MONITOR
        L_NETTRAFFIC_OK,
        L_NETTRAFFIC_CANCEL,


    gboxUnits->setTitle(lanmanager->getText(L_NETTRAFFIC_DATA_UNITS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    rdbkb->setText(lanmanager->getText(L_NETTRAFFIC_BITS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    rdbKB->setText(lanmanager->getText(L_NETTRAFFIC_BYTES,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));

    gboxShow->setTitle(lanmanager->getText(L_NETTRAFFIC_SHOW_UNIT_OR_NOT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    rdbShowUnit->setText(lanmanager->getText(L_NETTRAFFIC_SHOW_UNIT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    rdbNotShow->setText(lanmanager->getText(L_NETTRAFFIC_HIDE_UNIT,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));

//    lblUnits->setText(QApplication::translate("Widget", "Data transfer rate units:", 0, QApplication::UnicodeUTF8));
    //label_unit->setText(QApplication::translate("Widget", "Show unit or not:", 0, QApplication::UnicodeUTF8));
    gboxAdapters->setTitle(lanmanager->getText(L_NETTRAFFIC_NETWORK_ADAPTERS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    lblAdapter->setText(lanmanager->getText(L_NETTRAFFIC_WHICH_MONITOR,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    btnOk->setText(lanmanager->getText(L_NETTRAFFIC_OK,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    btnCancel->setText(lanmanager->getText(L_NETTRAFFIC_CANCEL,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
    this->setWindowTitle(lanmanager->getText(L_NETTRAFFIC_TITLE_OPTION,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")).trimmed());
    */
} // retranslateUi



void QGenie_qtOptions::okClicked(bool checked )
{
    if (qtNet)
    {
        /*
        if(comboBox->currentIndex()==0)
        {
            qtNet->iunit=0;
        }
        else
        {
            qtNet->iunit=1;
        }
        if(comboBox_show_unit->currentIndex()==0)
        {
            qtNet->iShowUnit=1;
        }
        else
        {
            qtNet->iShowUnit=0;
        }
        */
        if (rdbkb->isChecked()==true)
        {
            qtNet->iunit=0;
        }
        else
        {
            qtNet->iunit=1;
        }
        if (rdbShowUnit->isChecked()==true)
        {
            qtNet->iShowUnit=1;
        }
        else
        {
            qtNet->iShowUnit=0;
        }


        for (int i=0;i<treeAdapters->topLevelItemCount();i++)
        {
            if (i<20)
            {
                if (treeAdapters->topLevelItem(i)->checkState(0)==Qt::Checked)
                {
                    qtNet->infChoose[i]=1;
                }
                else
                {
                    qtNet->infChoose[i]=0;
                }
            }
            else
            {
                for (int j=0;j<20;j++)
                {
                    qtNet->infChoose[j]=1;

                }
                break;
            }

        }
        int iCount=0;
        for (int i=0;i<treeAdapters->topLevelItemCount();i++)
        {
            if(i<20)
            {
                if (treeAdapters->topLevelItem(i)->checkState(0)==Qt::Checked)
                {
                    iCount++;
                }
            }
        }

        if (iCount==0)
        {
            for (int j=0;j<20;j++)
            {
                qtNet->infChoose[j]=1;
            }
        }
    }

    //Qt::WindowFlags flags = 0;
   // flags=Qt::CustomizeWindowHint;
    //flags |= Qt::WindowStaysOnTopHint;
    if (qtNet->iMode==1)
    {
        //flags |=Qt::FramelessWindowHint;
        if (qtNet->iShowUnit==1)
        {
            qtNet->setFixedSize(150,17);

            qtNet->lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            qtNet->txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            qtNet->lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            qtNet->txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
        }
        else
        {
            qtNet->setFixedSize(100,17);

            qtNet->lblInput->setGeometry(3,this->geometry().height()-16,12,16);
            qtNet->txtInput->setGeometry(15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
            qtNet->lblOutput->setGeometry(this->geometry().width()/2+3,this->geometry().height()-16,12,16);
            qtNet->txtOutput->setGeometry(this->geometry().width()/2+15,this->geometry().height()-16,this->geometry().width()/2-15-3,16);
        }
        //qtNet->paintEvent();
    }
    //flags |=Qt::Tool;
    //qtNet->setWindowFlags(flags);

    qtNet->bOptPoped=false;

    //qtNet->setFocus(Qt::TabFocusReason);
    //
    //qtNet->repaint();
    qtNet->hide();
    qtNet->show();

    this->close();

}

void QGenie_qtOptions::cancelClicked(bool checked)
{
    Qt::WindowFlags flags = 0;
    flags=Qt::CustomizeWindowHint;
    flags |= Qt::WindowStaysOnTopHint;
    flags |=Qt::Tool;
    if (qtNet->iMode==1)
    {
        flags |=Qt::FramelessWindowHint;
    }
    qtNet->setWindowFlags(flags);

    qtNet->show();
    qtNet->bOptPoped=false;
    this->close();
}

void QGenie_qtOptions::closeEvent ( QCloseEvent * event )
{
    qtNet->bOptPoped=false;
}


void QGenie_qtOptions::changeEvent(QEvent *e)
{
   // ILanguageManager *lanmanager=WorkPanel::sHandler->getLanguageManager();
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
        retranslateUi(this);
        break;
    default:
        break;
    }
}
