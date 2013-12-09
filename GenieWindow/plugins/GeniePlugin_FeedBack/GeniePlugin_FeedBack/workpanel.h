#ifndef WORKPANEL_H
#define WORKPANEL_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCommandLinkButton>
#include <QVBoxLayout>
//#include "sslconnect.h"
#include <QStackedLayout>
#include "QProgressIndicator.h"
#include "qLineEditPlaceTxt.h"
#include "qTextEditPlaceTxt.h"
#include <QMap>
//////////////////////
#include "SendMailThread.h"
#include <QThread>
#include "genie2_interface.h"


//netgeargenie@netgear.com
//Wireless101

//const QString sName(QString::fromUtf8("netgeargenie@netgear.com"));
//const QString sPWD(QString::fromUtf8("Wireless101"));


//const QString sName(QString::fromUtf8("netgeargenie@gmail.com"));
 const QString sPWD(QString::fromUtf8("Siteview@NETGEAR@888"));  //Siteview@NETGEAR@888

//const QString sName(QString::fromUtf8("zhiguo.deng@dragonflow.com"));
//const QString sPWD(QString::fromUtf8("dddddd"));



class WorkPanel : public QWidget
{
    Q_OBJECT
public:
    WorkPanel(QWidget *parent);
        ~WorkPanel();

public:
    void setupUi();
    void retranslateUi();
    void changeEvent(QEvent *e);
    void resizeEvent ( QResizeEvent * event );
    void Genie_Log(QString s);

    void showEvent ( QShowEvent * event );
    void staReadFile();


    QTextEdit *textEdit;
   // QTextEdit *moni;
    QLabel *label;
    QLabel *label_2;
    QLabel * labelDesc;
    QLineEdit *emailAdd;
    QLineEdit *sjEdit;
    bool bLog;
    QLabel *lblWait;

    QMap<int,QString> qmapFrom;
    //qmapClickedCount;
    QString sName;
    //QString sPWD;
    QPushButton *sendBtn;
    QProgressIndicator* qPrg;

 ////   SslConnect *sslConn;
	/////////////////////
	SendMailThread sendThread;
	bool bRet;
	QString string;

    int sendFlag;
    QString sData;

    QVBoxLayout *vbox ;


    QWidget workWidget;
    QWidget *upWidget ;
    QWidget *bottomWidget;
    QLabel *l_Title;
    QPushButton *l_Info;
    QPushButton *clearBtn;
    QPushButton *l_Back;
    QVBoxLayout *aLayout;
    QString strNetError;
    QString strSubmitSuc;
    QString ErrMsg;

    QString strTxtEmail;
    QString strTxtSubject;
    QString strTxtFeedback;

    QWidget* qWidget_Mail;
    QWidget* qWidget_Finished;
    QStackedLayout* stackLy;

    int imsg;


public slots:
	void slot_Mail();
	void slot_sendMail();
    void slot_sendBtn();
    void slot_clearBtn();
    void BtnClicked( bool checked = false );
    void BtnBackClicked( bool checked = false );
    void textAnyChanged ( const QString & text );
    void textAnyChanged ( );
};

#endif // WORKPANEL_H
