#include  "QGenie_qtGraphColor.h"
#include "QGenie_qtNetTraffic.h"
#include "plugin_ui_def.h"
#include "ILanguageManager.h"
#include "LanguageNetTraffic.h"
#include <QEvent>



QGenie_qtGraphColor::QGenie_qtGraphColor(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    //this->setWindowTitle("Color Setting");

    //Qt::WindowFlags flags = 0;
    //flags=Qt::CustomizeWindowHint;

    //flags |=Qt::FramelessWindowHint;
   // flags |=Qt::Tool;
    //this->setWindowFlags(flags);

    this->setWindowIcon(QIcon(":/image/aa.ico"));
    SET_FIX_SIZE(NORMAL_BUTTON,btnColorOk);
    SET_FIX_SIZE(NORMAL_BUTTON,btnColorCancel);
    SET_FIX_SIZE(NORMAL_BUTTON,btnReset);

   // btnColorOk->setFocusPolicy(Qt::NoFocus);
  //  btnColorCancel->setFocusPolicy(Qt::NoFocus);
   // btnReset->setFocusPolicy(Qt::NoFocus);
}


void QGenie_qtGraphColor::setupUi(QWidget *Widget)
        {
            if (Widget->objectName().isEmpty())
                Widget->setObjectName(QString::fromUtf8("Widget"));
            Widget->resize(418, 416);
            Widget->setGeometry(350,200,418,416);
            Widget->setWindowFlags(Qt::WindowCloseButtonHint);
            frame = new QFrame(Widget);
            frame->setObjectName(QString::fromUtf8("frame"));
            frame->setGeometry(QRect(0, 10, 411, 331));
            frame->setFrameShape(QFrame::StyledPanel);
            frame->setFrameShadow(QFrame::Raised);
            lblColors = new QLabel(frame);
            lblColors->setObjectName(QString::fromUtf8("lblColors"));
            lblColors->setGeometry(QRect(10, 1, 121, 16));
            groupBox = new QGroupBox(frame);
            groupBox->setObjectName(QString::fromUtf8("groupBox"));
            groupBox->setGeometry(QRect(11, 20, 391, 221));

            //receive
            lblReceivedData = new QLabel(groupBox);
            lblReceivedData->setObjectName(QString::fromUtf8("lblReceivedData"));
            lblReceivedData->setGeometry(QRect(10, 25, 161, 21));
            cmbRec = new QComboBox(groupBox);
            cmbRec->setObjectName(QString::fromUtf8("cmbRec"));
            cmbRec->setGeometry(QRect(170, 20, 211, 22));
            initColorCombox(cmbRec);
            cmbRec->setCurrentIndex(9);

            //sent
            lblSentData = new QLabel(groupBox);
            lblSentData->setObjectName(QString::fromUtf8("lblSentData"));
            lblSentData->setGeometry(QRect(9, 55, 161, 21));
            cmbSent = new QComboBox(groupBox);
            cmbSent->setObjectName(QString::fromUtf8("cmbSent"));
            cmbSent->setGeometry(QRect(169, 50, 211, 22));
            initColorCombox(cmbSent);
            cmbSent->setCurrentIndex(28);

            //total
            lblTotalData = new QLabel(groupBox);
            lblTotalData->setObjectName(QString::fromUtf8("lblTotalData"));
            lblTotalData->setGeometry(QRect(9, 83, 161, 21));
            cmbTotal = new QComboBox(groupBox);
            cmbTotal->setObjectName(QString::fromUtf8("cmbTotal"));
            cmbTotal->setGeometry(QRect(169, 78, 211, 22));
            initColorCombox(cmbTotal);
            cmbTotal->setCurrentIndex(2);
            cmbTotal->setVisible(false);
            lblTotalData->setVisible(false);

            //graph background
            lblGraphBack = new QLabel(groupBox);
            lblGraphBack->setObjectName(QString::fromUtf8("lblGraphBack"));
            lblGraphBack->setGeometry(QRect(10, 113, 161, 21));
            cmbGraphBack = new QComboBox(groupBox);
            cmbGraphBack->setObjectName(QString::fromUtf8("cmbGraphBack"));
            cmbGraphBack->setGeometry(QRect(170, 108, 211, 22));
            initColorCombox(cmbGraphBack);
            cmbGraphBack->setCurrentIndex(32);

            //window background
            lblWinBack = new QLabel(groupBox);
            lblWinBack->setObjectName(QString::fromUtf8("lblWinBack"));
            lblWinBack->setGeometry(QRect(10, 141, 161, 21));
            cmbWinBack = new QComboBox(groupBox);
            cmbWinBack->setObjectName(QString::fromUtf8("cmbWinBack"));
            cmbWinBack->setGeometry(QRect(170, 136, 211, 22));
            initColorCombox(cmbWinBack);
            cmbWinBack->setCurrentIndex(18);


            btnReset = new QPushButton(groupBox);
            btnReset->setObjectName(QString::fromUtf8("btnReset"));
            btnReset->setGeometry(QRect(170, 180, 211, 23));
            connect(btnReset,SIGNAL(clicked ( bool )),this,SLOT(setToDefaults (bool)));


            gbxWinPro = new QGroupBox(frame);
            gbxWinPro->setObjectName(QString::fromUtf8("gbxWinPro"));
            gbxWinPro->setGeometry(QRect(10, 250, 391, 71));
            lblTran = new QLabel(gbxWinPro);
            lblTran->setObjectName(QString::fromUtf8("lblTran"));
            lblTran->setGeometry(QRect(10, 30, 151, 21));
            cmbTran = new QComboBox(gbxWinPro);
            cmbTran->setObjectName(QString::fromUtf8("cmbTran"));
            cmbTran->setGeometry(QRect(173, 30, 210, 22));
            cmbTran->addItem("Transparency 0% (Opacity)");
            cmbTran->addItem("Transparency 10%");
            cmbTran->addItem("Transparency 20%");
            cmbTran->addItem("Transparency 30%");
            cmbTran->addItem("Transparency 40%");
            cmbTran->addItem("Transparency 50%");
            cmbTran->addItem("Transparency 60%");
            cmbTran->addItem("Transparency 70%");
            cmbTran->addItem("Transparency 80% (Transparent)");
            //cmbTran->addItem("90%");
            //cmbTran->addItem("100%");
            cmbTran->setCurrentIndex(2);

            btnColorOk = new QPushButton(Widget);
            btnColorOk->setObjectName(QString::fromUtf8("btnColorOk"));
            btnColorOk->setGeometry(QRect(240, 380, 75, 23));
            connect(btnColorOk,SIGNAL(clicked ( bool  )),this,SLOT(okClicked (bool)));

            btnColorCancel = new QPushButton(Widget);
            btnColorCancel->setObjectName(QString::fromUtf8("btnColorCancel"));
            btnColorCancel->setGeometry(QRect(330, 380, 75, 23));
            connect(btnColorCancel,SIGNAL(clicked ( bool  )),this,SLOT(cancelClicked (bool)));


            QWidget::setTabOrder(cmbRec, btnColorOk);
            QWidget::setTabOrder(btnColorOk, btnColorCancel);

            retranslateUi(Widget);

            QMetaObject::connectSlotsByName(Widget);
        } // setupUi

  void QGenie_qtGraphColor::retranslateUi(QWidget *Widget)
        {
       // ILanguageManager *lanmanager=qtNet->sHandler->getLanguageManager();
      //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
      /*
        L_NETTRAFFIC_BIGBT=2000,
        L_NETTRAFFIC_LEFTBT,
        L_NETTRAFFIC_COLOR_SETTING,
        L_NETTRAFFIC_COLORS,
        L_NETTRAFFIC_GRAPH_COLORS,
        L_NETTRAFFIC_R_DATA,
        L_NETTRAFFIC_SENT_DATA,
        L_NETTRAFFIC_TOTAL_DATA,

        L_NETTRAFFIC_GRAPH_BACKGROUND,
        L_NETTRAFFIC_WINDOW_BACKGROUND,
        L_NETTRAFFIC_RESET_DEFAULTS,
        L_NETTRAFFIC_WINDOW_PROPERTIES,
        L_NETTRAFFIC_WINDOW_TRANSLUCENCY,
        L_NETTRAFFIC_OK,
        L_NETTRAFFIC_CANCEL,


            Widget->setWindowTitle(lanmanager->getText(L_NETTRAFFIC_COLOR_SETTING,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblColors->setText(lanmanager->getText(L_NETTRAFFIC_COLORS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            groupBox->setTitle(lanmanager->getText(L_NETTRAFFIC_GRAPH_COLORS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblReceivedData->setText(lanmanager->getText(L_NETTRAFFIC_R_DATA,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblSentData->setText(lanmanager->getText(L_NETTRAFFIC_SENT_DATA,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblTotalData->setText(lanmanager->getText(L_NETTRAFFIC_TOTAL_DATA,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));

            lblGraphBack->setText(lanmanager->getText(L_NETTRAFFIC_GRAPH_BACKGROUND,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblWinBack->setText(lanmanager->getText(L_NETTRAFFIC_WINDOW_BACKGROUND,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            btnReset->setText(lanmanager->getText(L_NETTRAFFIC_RESET_DEFAULTS,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            gbxWinPro->setTitle(lanmanager->getText(L_NETTRAFFIC_WINDOW_PROPERTIES,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            lblTran->setText(lanmanager->getText(L_NETTRAFFIC_WINDOW_TRANSLUCENCY,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            btnColorOk->setText(lanmanager->getText(L_NETTRAFFIC_OK,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            btnColorCancel->setText(lanmanager->getText(L_NETTRAFFIC_CANCEL,QUuid("{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}")));
            */
        } // retranslateUi



  void QGenie_qtGraphColor::okClicked(bool checked )
  {
      if (qtNet)
      {
          qtNet->qclrReceive=getColorFromComboBox(cmbRec);
          qtNet->qclrSent=getColorFromComboBox(cmbSent);
          qtNet->qclrTotal=getColorFromComboBox(cmbTotal);
          qtNet->qclrGraphBg=getColorFromComboBox(cmbGraphBack);
          qtNet->qclrWindowBg=getColorFromComboBox(cmbWinBack);
          qtNet->dbOpt=getDblFromComboBox(cmbTran);
          qtNet->setWindowOpacity(qtNet->dbOpt);
      }


      Qt::WindowFlags flags = 0;
      flags=Qt::CustomizeWindowHint;
      flags |= Qt::WindowStaysOnTopHint;
      flags |=Qt::Tool;
      if (qtNet->iMode==1)
      {
          flags |=Qt::FramelessWindowHint;
      }
      //qtNet->setWindowFlags(flags);

      //this->hide();

      //qtNet->hide();
      qtNet->show();
      qtNet->bColorPoped=false;
      this->close();
  }

  double QGenie_qtGraphColor::getDblFromComboBox(QComboBox* qcom)
  {
      double dbl=0.2;
      if (qcom)
      {
          QString str=qcom->currentText();


           /*
            cmbTran->addItem("Transparency 0% (Opacity)");
            cmbTran->addItem("Transparency 10%");
            cmbTran->addItem("Transparency 20%");
            cmbTran->addItem("Transparency 30%");
            cmbTran->addItem("Transparency 40%");
            cmbTran->addItem("Transparency 50%");
            cmbTran->addItem("Transparency 60%");
            cmbTran->addItem("Transparency 70%");
            cmbTran->addItem("Transparency 80% (Transparent)");
            */

          if (str== "Transparency 0% (Opacity)")
              dbl=0.0;
          if (str== "Transparency 10%")
              dbl=0.1;
          if (str== "Transparency 20%")
              dbl=0.2;
          if (str== "Transparency 30%")
              dbl=0.3;
          if (str== "Transparency 40%")
              dbl=0.4;
          if (str== "Transparency 50%")
              dbl=0.5;
          if (str== "Transparency 60%")
              dbl=0.6;
          if (str== "Transparency 70%")
              dbl=0.7;
          if (str== "Transparency 80% (Transparent)")
              dbl=0.8;


      }
      return 1-dbl;
  }

  QColor QGenie_qtGraphColor::getColorFromComboBox(QComboBox* qcom)
  {
      QColor qclr=QColor();
      int r,g,b;
      r=0;
      g=0;
      b=0;

      if (qcom)
      {
          QString str=qcom->currentText();
          int iposR=str.indexOf("R");
          QString red=str.mid(iposR+1,3);
          int iposG=str.indexOf("G");
          QString green=str.mid(iposG+1,3);
          int iposB=str.indexOf("B");
          QString blue=str.mid(iposB+1,3);

          r=red.toInt();
          g=green.toInt();
          b=blue.toInt();
          qclr=QColor(r,g,b);
          return qclr;
      }
      return qclr;
  }

  void QGenie_qtGraphColor::cancelClicked(bool checked)
  {
      //listAllConnections();
      Qt::WindowFlags flags = 0;
      flags=Qt::CustomizeWindowHint;
      flags |= Qt::WindowStaysOnTopHint;
      flags |=Qt::Tool;
      if (qtNet->iMode==1)
      {
          flags |=Qt::FramelessWindowHint;
      }
      qtNet->setWindowFlags(flags);
      /*
      qtNet->txtInput->setAlignment(Qt::AlignHCenter);
      qtNet->txtOutput->setAlignment(Qt::AlignHCenter);
      qtNet->txtInput->setAlignment(Qt::AlignBottom);
      qtNet->txtOutput->setAlignment(Qt::AlignBottom);
      */
      qtNet->show();
      qtNet->bColorPoped=false;
      this->close();
  }

  //setToDefaults (bool)
  void QGenie_qtGraphColor::setToDefaults(bool checked)
  {
      //listAllConnections();
      //this->close();
      cmbRec->setCurrentIndex(15);
      cmbSent->setCurrentIndex(28);
      cmbTotal->setCurrentIndex(2);
      cmbGraphBack->setCurrentIndex(32);
      cmbWinBack->setCurrentIndex(19);
  }


 void QGenie_qtGraphColor::initColorCombox(QComboBox* qcom)
 {
    if (qcom)
    {
        qcom->clear();
        QColor qclr;
        QPixmap qpic=QPixmap(11,11);
        int r,g,b;
        QIcon qicon=QIcon();

        //1
        r=153;
        g=102;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G102 B000");


        //2
        r=204;
        g=153;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R204 G153 B000");

        //3
        r=255;
        g=204;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G204 B000");

        //4
        r=255;
        g=255;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G255 B000");

        //5
        r=255;
        g=255;
        b=153;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G255 B153");

        //6
        r=255;
        g=219;
        b=157;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G219 B157");

        //7
        r=255;
        g=204;
        b=102;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G204 B102");

        //8
        r=255;
        g=153;
        b=51;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G153 B051");

        //9
        r=255;
        g=121;
        b=75;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G121 B075");

        //10
        r=255;
        g=51;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G051 B000");

        //11
        r=153;
        g=102;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G102 B000");

        //12
        r=153;
        g=0;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G000 B000");

        //13
        r=51;
        g=51;
        b=102;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R051 G051 B102");

        //14
        r=0;
        g=51;
        b=153;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G051 B153");

        //15
        r=0;
        g=102;
        b=204;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G102 B204");

        //16
        r=0;
        g=131;
        b=215;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G131 B215");

        //17
        r=000;
        g=153;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G153 B255");

        //18
        r=62;
        g=154;
        b=222;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R062 G154 B222");

        //19
        r=153;
        g=204;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G204 B255");

        //20
        r=180;
        g=226;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R180 G226 B255");

        //21
        r=222;
        g=255;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R222 G255 B255");


        //22
        r=255;
        g=204;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R255 G204 B255");

        //23
        r=204;
        g=204;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R204 G204 B255");

        //24
        r=153;
        g=153;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G153 B255");

        //25
        r=102;
        g=102;
        b=204;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R102 G102 B204");

        //26
        r=153;
        g=153;
        b=204;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G153 B204");

        //27
        r=102;
        g=102;
        b=153;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R102 G102 B153");

        //28
        r=0;
        g=102;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G102 B000");

        //29
        r=0;
        g=153;
        b=0;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R000 G153 B000");

        //30
        r=102;
        g=204;
        b=51;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R102 G204 B051");

        //31
        r=153;
        g=255;
        b=102;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R153 G255 B102");

        //32
        r=204;
        g=255;
        b=204;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R204 G255 B204");

        //33
        r=222;
        g=255;
        b=255;
        qclr=QColor(r,g,b);
        qpic.fill(qclr);
        qicon.addPixmap(qpic);
        qcom->addItem(qicon,"       R222 G255 B255");

    }
 }

 void QGenie_qtGraphColor::closeEvent ( QCloseEvent * event )
 {
     qtNet->bColorPoped=false;
 }


 void QGenie_qtGraphColor::changeEvent(QEvent *e)
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
