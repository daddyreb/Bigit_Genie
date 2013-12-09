#include "workpanel.h"
#include <QHBoxlayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QEvent>
#include <QGridLayout>
#include <QTextCodec>
//#include "math.h"
#include <QFile>
#include "language_FeedBack.h"


#define SEND_TO "netgeargenie@netgear.com"

WorkPanel::WorkPanel(QWidget *parent)
    :QWidget(parent)
{
    bLog=true;
    imsg=1;
    sData = "";
    sendFlag = -1;
    this->setupUi();

        bRet = false;

    connect(emailAdd,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));
    connect(sjEdit,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));
    connect(textEdit,SIGNAL(textChanged()),this,SLOT(textAnyChanged()));
    connect(&sendThread, SIGNAL(finished()), this, SLOT(slot_Mail()));


    clearBtn->setEnabled(false);
    sendBtn->setEnabled(false);

    qmapFrom.clear();
    staReadFile();


}

WorkPanel::~WorkPanel()
{
	
}
void WorkPanel::slot_sendBtn()
{
    Genie_Log("slot_sendBtn() started.");
    GENIE2_STATISTICS("F1");

    clearBtn->setEnabled(false);
    sendBtn->setEnabled(false);
////    QString hostname = QString::fromUtf8("smtp.gmail.com");
    //QString hostname = QString::fromUtf8("mail.netgear.com");
   // QString hostname = QString::fromUtf8("mail.dragonflow.com");


/////    int portnum = 465;//465;

    //int portnum = 1469;

    //int portnum = 25;
    //qPrg->setVisible(true);
    //qPrg=new QProgressIndicator(this);
    //qPrg->setDisplayedWhenStopped(false);
    //qPrg->setGeometry((this->width())/2-25,(this->height())/2-25,50,50);
   // stackLy->setCurrentIndex(2);

    lblWait->setVisible(true);
    qPrg->startAnimation();

    Genie_Log("select mailbox started.");
    //Ñ¡Ôñ·¢ËÍÓÊÏä
    int iMailCount=qmapFrom.count();
    int iRan=0;
    if (iMailCount>0)
    {
        iRan=qrand()%iMailCount;
        sName=qmapFrom.values().at(iRan);
        qmapFrom.remove(qmapFrom.keys().at(iRan));
    }
    else
    {
        sName="netgeargenie@gmail.com";  //netgeargenie@gmail.com
        staReadFile();
    }
    Genie_Log("select mailbox ended.");
	
	slot_sendMail();
	
}



void WorkPanel::slot_sendMail()
{
	QByteArray bytename = sName.toLocal8Bit();
	const char *cname = bytename.data();


	QString stremailadd = emailAdd->text().trimmed();
	if(stremailadd.length() > 128)
	{
		stremailadd = stremailadd.left(128);
	}
	QByteArray bytemailadd = stremailadd.toLocal8Bit();
	const char *cemailadd = bytemailadd.data();

	QString strsubject = sjEdit->text().trimmed();
	if(strsubject.length() > 128)
	{
		strsubject = strsubject.left(128);
	}
	QByteArray bytesubject = strsubject.toLocal8Bit();
	const char *csubject = bytesubject.data();

	QString strneirong = textEdit->toPlainText().trimmed();
	if(strneirong.length() > 512)
	{
		strneirong = strneirong.left(512);
	}

	sData.clear();
	sData +="Mail from " +stremailadd+";\nMail subject is " + strsubject +";\n"+ strneirong+"\n";
	QByteArray byteData = sData.toLocal8Bit();
	const char *cData = byteData.data();

	sendThread.runSend(csubject, cname, SEND_TO, cData, 0);

	Genie_Log(QString("sendData start."));
	Genie_Log(sData);
		
}

void WorkPanel::slot_Mail()
{
	bRet = sendThread.mRet;
	if(!bRet && (qmapFrom.count() > 0))
	{
		slot_sendBtn();
		return;
	}
	if(!bRet)
	{
		if(!sendThread.errInfo.compare("Invalid winsock2 socket\n"))
		{
			    l_Title->setText(strNetError);
				imsg=1;
				Genie_Log(strNetError);
				lblWait->setVisible(false);
				qPrg->stopAnimation();
				stackLy->setCurrentIndex(1);
				clearBtn->setEnabled(true);
				sendBtn->setEnabled(true);
		}
		else
		{
				l_Title->setText(ErrMsg);
				imsg = 2;

				lblWait->setVisible(false);
				qPrg->stopAnimation();
				stackLy->setCurrentIndex(1);
				sendBtn->setEnabled(true);
				clearBtn->setEnabled(true);
		}
	}
	else
	{
		l_Title->setText(strSubmitSuc);
		imsg = 3;
		lblWait->setVisible(false);
		qPrg->stopAnimation();
		stackLy->setCurrentIndex(1);
	}
	Genie_Log(sendThread.errInfo);
}

void	WorkPanel::textAnyChanged ( const QString & text )
{
    if (emailAdd->text()=="" && sjEdit->text()=="" && textEdit->toPlainText()=="")
    {
        clearBtn->setEnabled(false);
    }
    else
    {
        clearBtn->setEnabled(true);
    }

    /*
    emailAdd->setText(strTxtEmail);
    sjEdit->setText(strTxtSubject);
    textEdit->setPlainText(strTxtFeedback);
    */
    if (emailAdd->text()=="" || sjEdit->text()=="" || textEdit->toPlainText()=="" || emailAdd->text()==strTxtEmail || sjEdit->text()==strTxtSubject || textEdit->toPlainText()==strTxtFeedback)
    {
        sendBtn->setEnabled(false);
    }
    else
    {
        sendBtn->setEnabled(true);
    }
}

void	WorkPanel::textAnyChanged ( )
{
    if (emailAdd->text()=="" && sjEdit->text()=="" && textEdit->toPlainText()=="")
    {
        clearBtn->setEnabled(false);
    }
    else
    {
        clearBtn->setEnabled(true);
    }

    if (emailAdd->text()=="" || sjEdit->text()=="" || textEdit->toPlainText()=="" || emailAdd->text()==strTxtEmail || sjEdit->text()==strTxtSubject || textEdit->toPlainText()==strTxtFeedback)
    {
        sendBtn->setEnabled(false);
    }
    else
    {
        sendBtn->setEnabled(true);
    }
}


void WorkPanel::setupUi()
{

    //-----------/Layout-1/----------------------------

   // panel->setStyleSheet("background-color: rgb(255, 85, 0);");

    QWidget *panel = new QWidget(this);
    panel->setObjectName(QString::fromUtf8("panel"));


    QHBoxLayout *hbox = new QHBoxLayout(panel);
    hbox->setObjectName(QString::fromUtf8("hbox"));
    hbox->setContentsMargins(0,0,0,0);

   // hbox->setSpacing(0);


    QWidget *panelfour = new QWidget(this);
    panelfour->setObjectName(QString::fromUtf8("panelfour"));


    QVBoxLayout *vboxfour = new QVBoxLayout(panelfour);
    vboxfour->setObjectName(QString::fromUtf8("vboxfour"));
    vboxfour->setContentsMargins(10,0,10,0);


    label = new QLabel(panelfour);
    label->setObjectName(QString::fromUtf8("label"));
    label->setMaximumWidth(270);
    label->setMinimumWidth(270);


    emailAdd = new QLineEdit(panelfour);
    emailAdd->setObjectName(QString::fromUtf8("emailAdd"));
    emailAdd->setMaximumWidth(270);
    emailAdd->setMinimumWidth(270);
    emailAdd->setMinimumHeight(25);
    

    label_2 = new QLabel(panelfour);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setMaximumWidth(270);
    label_2->setMinimumWidth(270);

    sjEdit = new QLineEdit(panelfour);
    sjEdit->setObjectName(QString::fromUtf8("sjEdit"));
    sjEdit->setMaximumWidth(270);
    sjEdit->setMinimumWidth(270);
    sjEdit->setMinimumHeight(25);

    vboxfour->addWidget(label);
    vboxfour->addWidget(emailAdd);
    vboxfour->addWidget(label_2);
    vboxfour->addWidget(sjEdit);



    QWidget *panelprg = new QWidget(this);
    panelprg->setObjectName(QString::fromUtf8("panelprg"));


    QVBoxLayout *vboxprg = new QVBoxLayout(panelprg);
    vboxprg->setObjectName(QString::fromUtf8("vboxprg"));
    vboxprg->setContentsMargins(10,0,10,0);



    QWidget *panelprghbox = new QWidget(panelprg);

    QHBoxLayout *hboxprg = new QHBoxLayout(panelprghbox);
    hboxprg->setObjectName(QString::fromUtf8("hboxprg"));
    hboxprg->setContentsMargins(0,0,0,0);


    qPrg=new QProgressIndicator(panelprghbox);
    qPrg->setDisplayedWhenStopped(false);

    hboxprg->insertStretch(0,160);
    hboxprg->addWidget(qPrg);
    hboxprg->insertStretch(2,160);

    lblWait=new QLabel(panelprg);
    lblWait->setVisible(false);
    //lblWait->setFixedWidth(320);
    lblWait->setAlignment(Qt::AlignCenter);
    lblWait->setWordWrap(true);

    vboxprg->addWidget(panelprghbox);
    vboxprg->addWidget(lblWait);


    hbox->addWidget(panelfour);
    hbox->addWidget(panelprg);



    //-----------/Layout-3----------------------------------
    QWidget *panel_3 = new QWidget(this);
    panel_3->setObjectName(QString::fromUtf8("panel_3"));

    QHBoxLayout *hbox_3 = new QHBoxLayout(panel_3);
    hbox_3->setObjectName(QString::fromUtf8("hbox_3"));
   // hbox_3->setContentsMargins(10,0,10,0);
    hbox_3->setContentsMargins(10,0,10,0);
    hbox_3->setSpacing(0);

    textEdit = new QTextEdit(panel_3);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    hbox_3->addWidget(textEdit);


    //-----------/Layout-4----------------------------------
    QWidget *panel_4 = new QWidget(this);
    panel_4->setObjectName(QString::fromUtf8("panel_4"));
    //panel_4->setMaximumHeight(50);
   // panel_4->setMinimumHeight(50);
   // panel_4->setStyleSheet("background-color: rgb(85, 255, 0);");

    QHBoxLayout *hbox_4 = new QHBoxLayout(panel_4);
    hbox_4->setObjectName(QString::fromUtf8("hbox_4"));
    hbox_4->setContentsMargins( 10,0,10,4);
    hbox_4->setSpacing(0);

    //clearBtn

    clearBtn = new QPushButton(panel_4);
    clearBtn->setFocusPolicy(Qt::TabFocus);
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(slot_clearBtn()));

    sendBtn = new QPushButton(panel_4);
    sendBtn->setFocusPolicy(Qt::TabFocus);
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(slot_sendBtn()));

    hbox_4->insertStretch(0,500);
    hbox_4->addWidget(clearBtn);
    hbox_4->insertStretch(2,20);
    hbox_4->addWidget(sendBtn);
    hbox_4->insertStretch(4,20);

    //--------------------------------------------------------

    //QVBoxLayout *
    vbox = new QVBoxLayout(this);
    vbox->setObjectName(QString::fromUtf8("vbox"));
    vbox->setContentsMargins(5,20,5,5);
   // vbox->setSpacing(1);

    //vbox->addWidget(panel);
    labelDesc=new QLabel();
    labelDesc->setWordWrap(true);
    labelDesc->setOpenExternalLinks(true);


    QWidget *panel_Desc = new QWidget(this);
    panel_Desc->setObjectName(QString::fromUtf8("panel_Desc"));

    QHBoxLayout *hbox_Desc = new QHBoxLayout(panel_Desc);
    hbox_Desc->setObjectName(QString::fromUtf8("hbox_Desc"));
    hbox_Desc->setContentsMargins(10,0,10,10);
    hbox_Desc->setSpacing(0);

    //textEdit = new QTextEdit(panel_3);
    //textEdit->setObjectName(QString::fromUtf8("textEdit"));
    hbox_Desc->addWidget(labelDesc);



    vbox->addWidget(panel_Desc);
    vbox->addWidget(panel);

    vbox->addWidget(panel_3);
    vbox->addWidget(panel_4);


    /////////////////////////////////////////////// add status window
    upWidget = new QWidget(this);
    QHBoxLayout *uwLayout = new QHBoxLayout(upWidget);   
    uwLayout->setContentsMargins(0,0,0,0);


    l_Title = new QLabel(upWidget);
    l_Title->setObjectName(QString::fromUtf8("l_Title"));
    //l_Title->setWordWrap(true); //l_Title->setFrameShape(QFrame::Panel);
    uwLayout->insertStretch(0,500);
    uwLayout->addWidget(l_Title);
    uwLayout->insertStretch(2,500);
   //---create bottom widget for show status and add a button ,startup live parental control tool----
    bottomWidget = new QWidget(this);
    bottomWidget->setObjectName(QString::fromUtf8("bottomWidget"));
    bottomWidget->setMinimumHeight(30);

    QHBoxLayout *bwLayout = new QHBoxLayout(bottomWidget);
    bwLayout->setObjectName(QString::fromUtf8("bwLayout"));
    bwLayout->setContentsMargins(0,0,0,0);


    //l_Back
    l_Back = new QPushButton(bottomWidget);
    l_Back->setFocusPolicy(Qt::TabFocus);
    l_Back->setObjectName(QString::fromUtf8("l_Info"));

    connect(l_Back,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));

    l_Info = new QPushButton(bottomWidget);
    l_Info->setFocusPolicy(Qt::TabFocus);
    l_Info->setObjectName(QString::fromUtf8("l_Info"));

    connect(l_Info,SIGNAL(clicked( bool)),this,SLOT(BtnClicked( bool)));

    bwLayout->insertStretch(0,500);
    bwLayout->addWidget(l_Back);
    bwLayout->addWidget(l_Info);
    bwLayout->insertStretch(3,500);

    //-----Layout all --------------------------------------

    aLayout = new QVBoxLayout(this);
    aLayout->setObjectName(QString::fromUtf8("aLayout"));
    aLayout->setContentsMargins(0,10,0,10);
    aLayout->setSpacing(50);

    aLayout->insertStretch(0,500);
    aLayout->addWidget(upWidget);
    aLayout->insertStretch(2,20);
    aLayout->addWidget(bottomWidget);
    aLayout->insertStretch(4,500);


    qWidget_Finished=new QWidget(this);
    qWidget_Finished->setLayout(aLayout);


    /////////////////////////////////////////////// add status window end

    qWidget_Mail=new QWidget(this);
    qWidget_Mail->setLayout(vbox);


    stackLy=new QStackedLayout(this);
    stackLy->addWidget(qWidget_Mail);
    stackLy->addWidget(qWidget_Finished);
    
   // QGridLayout * lyPrg = new QGridLayout(this);
    //lyPrg->addWidget(qPrg,0,0,1,1);
  //  QWidget* wgtPrg=new QWidget(this);
   // wgtPrg->setLayout(lyPrg);
    
    //qPrg->setGeometry((this->width())/2-25,(this->height())/2-25,50,50);
    //stackLy->addWidget(wgtPrg);

    this->setLayout(stackLy);
    stackLy->setCurrentIndex(0);

    retranslateUi();
    //this->initialSslConn();

}




void WorkPanel::Genie_Log(QString s)
{
    if (bLog)
    {
        GENIE2_LOG(s);
    }
}

void WorkPanel::resizeEvent ( QResizeEvent * event )
{
    QWidget::resizeEvent(event);
}

void WorkPanel::retranslateUi()
{
    label->setText(GENIE2_GET_TEXT(L_FEEDBACK_LABLE_FROM));//lanmanager->getText(L_FEEDBACK_LABLE_FROM,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    label_2->setText(GENIE2_GET_TEXT(L_FEEDBACK_LABLE_SUB));//lanmanager->getText(L_FEEDBACK_LABLE_SUB,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    sendBtn->setText(GENIE2_GET_TEXT(L_FEEDBACK_BTN_SEND));//lanmanager->getText(L_FEEDBACK_BTN_SEND,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    clearBtn->setText(GENIE2_GET_TEXT(L_FEEDBACK_BTN_CLEAR));//lanmanager->getText(L_FEEDBACK_BTN_CLEAR,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    l_Back->setText(GENIE2_GET_TEXT(L_FEEDBACK_BTN_BACK));//lanmanager->getText(L_FEEDBACK_BTN_BACK,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    l_Info->setText(GENIE2_GET_TEXT(L_FEEDBACK_BTN_FINISHED));//lanmanager->getText(L_FEEDBACK_BTN_FINISHED,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));
    lblWait->setText(GENIE2_GET_TEXT(L_FEEDBACK_LABEL_WAIT));//lanmanager->getText(L_FEEDBACK_LABEL_WAIT,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));

    labelDesc->setText(GENIE2_GET_TEXT(L_FEEDBACK_LABEL_DESC));//lanmanager->getText(L_FEEDBACK_LABEL_DESC,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}")));


     strNetError=GENIE2_GET_TEXT(L_FEEDBACK_LABEL_ERR_NET);//lanmanager->getText(L_FEEDBACK_LABEL_ERR_NET,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}"));
     strSubmitSuc=GENIE2_GET_TEXT(L_FEEDBACK_LABEL_SUB_SUC);//lanmanager->getText(L_FEEDBACK_LABEL_SUB_SUC,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}"));
     ErrMsg=GENIE2_GET_TEXT(L_FEEDBACK_LABEL_ERR_MSG);//lanmanager->getText(L_FEEDBACK_LABEL_ERR_MSG,QUuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}"));

     if (imsg==1)
     {
         l_Title->setText(strNetError);
     }
     if (imsg==2)
     {
         l_Title->setText(ErrMsg);
     }
     if (imsg==3)
     {
         l_Title->setText(strSubmitSuc);
     }
}

void	WorkPanel::showEvent ( QShowEvent * event )
{
}


void WorkPanel::slot_clearBtn()
{
    emailAdd->setText("");
    sjEdit->setText("");
    textEdit->setText("");
    //initialTxt();
}


void WorkPanel::BtnClicked( bool checked  )
{
    lblWait->setVisible(false);
    qPrg->stopAnimation();
    sendBtn->setEnabled(true);


    slot_clearBtn();
    //initialTxt();
    stackLy->setCurrentIndex(0);

    QVariantList lst;
    lst<<-1<<0;
    GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,lst);
}

void WorkPanel::BtnBackClicked( bool checked  )
{
    lblWait->setVisible(false);
    qPrg->stopAnimation();
    stackLy->setCurrentIndex(0);
    sendBtn->setEnabled(true);
    clearBtn->setEnabled(true);
}


void WorkPanel::changeEvent(QEvent *e)
{

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


void WorkPanel::staReadFile()
{
    Genie_Log("staReadFile start.");
    QString fileName = "maildb.dat";
    if (!QFile::exists(fileName))
    {
        return;
    }

    QFile* file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        delete file;
        file = 0;
        return;
    }

     QDataStream in(file);
     in>>qmapFrom;

     //QString fileName = "function_statistics.dat";
     file->close();
     delete file;
     Genie_Log("staReadFile end.");
}
