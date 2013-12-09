#include "QGenieRSBasic.h"
#include <QEvent>
#include <QFont>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <QMovie>
#include <QUiLoader>
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "pcpform.h"
#include "QGenieRCWirelessObject.h"
extern quint64 sRCFlags;

QGenieRSBasic::QGenieRSBasic()
    :movie(0)
    ,mW(NULL)
{
    bAlarm=false;
//    setupUi();

    mFileCheckProcess=new QProcess;
    connect(mFileCheckProcess,SIGNAL(finished(int)),this,SLOT(slot_ProcessEnd(int)));
    bFirstShow=true;

}


QGenieRSBasic::~QGenieRSBasic()
{
    mFileCheckProcess->close();
    GENIE2_SAFEDELETE(mFileCheckProcess);
    GENIE2_SAFEDELETE(mW);
    GENIE2_SAFEDELETE(movie);
}





QWidget * QGenieRSBasic::getUi()
{
    if(!mW)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/readysharebasic.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, NULL);
        Q_ASSERT(w);

        lblReadyShare=w->findChild<QLabel *>("lab_title");
        lblDevice=w->findChild<QLabel *>("lab_device");
        lblDevicePash=w->findChild<QLabel *>("lab_pash");
        lblAlarm=w->findChild<QLabel *>("lab_alarm");
        lblPic=w->findChild<QLabel *>("lab_pic");
        copyLink = w->findChild<QPushButton *>("copyLinkBtn");
        btnBack=w->findChild<QPushButton *>("but_back");
        Q_ASSERT(lblReadyShare && lblDevice && lblDevicePash &&lblAlarm
                 &&lblPic &&lblPic &&btnBack);
        copyLink->setVisible(false);///remove copy link button

        const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
        QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                    "<html>"
                        "<head>"
                            "<meta name=\"qrichtext\" content=\"1\" />"
                                "<style type=\"text/css\">"
                                    "p, li { white-space: pre-wrap; }"
                                "</style>"
                        "</head>"
                        "<body style=\" font-family:'ËÎÌו'; font-size:12pt; font-weight:400; font-style:normal;\">"
                            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
   #ifdef Q_OS_WIN32
                                "<a href=\"file:\\\\%1\\\">"
                                "<span style=\" text-decoration: underline; color:#0000ff;\">\\\\%2"

   #else
                                "<a href=\"smb://%1\">"
                                "<span style=\" text-decoration: underline; color:#0000ff;\">smb://%2"

   #endif
                                    "</span>"
                                 "</a>"
                            "</p>"
                        "</body>"
                        "</html>").arg(strIp).arg(strIp);

        lblDevicePash->setText(str);
        connect(lblDevicePash,SIGNAL(linkActivated(QString)),this,SLOT(linkActivated(QString)));
        setMovie(GENIE2_RES("images/readyshare/readyshare_Basic.png"));
        connect(btnBack,SIGNAL(clicked()),this,SLOT(BtnBackClicked()));
//        connect(copyLink,SIGNAL(clicked()),this,SLOT(BtnCopyLink()));

        QFrame *fr_pi=w->findChild<QFrame *>("fr_pi");
        Q_ASSERT(fr_pi);
        qPrg=new QProgressIndicator();
        QHBoxLayout *layout=new QHBoxLayout(fr_pi);
		qPrg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(qPrg);
        mW=w;
        retranslateUi();
    }
    return mW;
}

void QGenieRSBasic::getFileExist(bool bFileExist)
{
//    QMessageBox::warning(NULL,"",bFileExist?"exist":"not exist");
    if (bFileExist)
    {
        const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
        bAlarm=false;
        lblAlarm->setText("");
        if (!bFirstShow)
        {
#ifdef Q_OS_WIN32
            QString strUrl=QString("file:\\\\%1\\").arg(strIp);
#else
            QString strUrl=QString("smb://%1/").arg(strIp);
#endif
            QDesktopServices::openUrl(QUrl(strUrl, QUrl::TolerantMode));
        }

        setMovie(GENIE2_RES("images/readyshare/readyshare_Basic.png"));

        bFirstShow=false;
    }
    else
    {
        bAlarm=true;
        lblAlarm->setText(GENIE2_GET_TEXT(L_READY_SHARE_TRYAGAIN ));

        setMovie(GENIE2_RES("images/readyshare/readyshare_Basic.gif"));

        bFirstShow=false;
    }
    qPrg->stopAnimation();
}

void QGenieRSBasic::tryStartFileCheckProcess()
{
//    QMessageBox::warning(NULL,"","QGenieRSBasic::tryStartFileCheckProcess()");
    if(mFileCheckProcess && !(sRCFlags &RCF_READYSHARE_PROCESS_ON))
    {
        QStringList paras;
        const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
        paras<<"//"+strIp + "/USB_Storage";
        qDebug()<<"tryStartFileCheckProcess:"<<paras;
        qDebug()<<qApp->applicationDirPath()+"/ReadyShareFileChecker";
        mFileCheckProcess->start(qApp->applicationDirPath()+"/ReadyShareFileChecker",paras);
        sRCFlags |= RCF_READYSHARE_PROCESS_ON;
        qPrg->startAnimation();
    }
}

void QGenieRSBasic::setMovie(const QString &fname)
{
    if(movie)
    {
        movie->deleteLater();
    }
    movie=new QMovie(fname);
    lblPic->setMovie(movie);
    movie->start();
}

void QGenieRSBasic::linkActivated ( const QString & link )
{
	Q_UNUSED(link);
    lblAlarm->setText("");
    tryStartFileCheckProcess();
    ///
    GENIE2_STATISTICS("R15");
}

void QGenieRSBasic::ipChanged(const QString &strIp)
{

    QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                "<html>"
                    "<head>"
                        "<meta name=\"qrichtext\" content=\"1\" />"
                            "<style type=\"text/css\">"
                                "p, li { white-space: pre-wrap; }"
                            "</style>"
                    "</head>"
                    "<body style=\" font-family:'ËÎÌו'; font-size:12pt; font-weight:400; font-style:normal;\">"
                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
#ifdef Q_OS_WIN32
                            "<a href=\"file:\\\\%1\\\">"
                            "<span style=\" text-decoration: underline; color:#0000ff;\">\\\\%2"

#else
                            "<a href=\"smb://%1\">"
                            "<span style=\" text-decoration: underline; color:#0000ff;\">smb://%2"
#endif
                                 "</span>"
                             "</a>"
                        "</p>"
                    "</body>"
                "</html>").arg(strIp).arg(strIp);

    lblDevicePash->setText(str);

    if (bFirstShow)
    {
        lblAlarm->setText("");
        tryStartFileCheckProcess();

    }

}


void QGenieRSBasic::BtnBackClicked()
{
    bFirstShow=true;
    setMovie(GENIE2_RES("images/readyshare/readyshare_Basic.png"));

    PCPForm::GetSingletonPtr()->showPreFrom();
}



//void QGenieRSBasic::changeEvent(QEvent *e)
//{

//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        retranslateUi();
//        break;
//    default:

//        break;
//    }
//}


void QGenieRSBasic::retranslateUi()
{
    if(mW)
    {
        lblReadyShare->setText("<b>"+GENIE2_GET_TEXT(L_READY_SHARE_FUN_ONE )+"</b>");

        lblDevice->setText(GENIE2_GET_TEXT(L_READY_SHARE_NETWORK_NAME ));
        btnBack->setText(GENIE2_GET_TEXT(L_READY_SHARE_BACK ));
        //kai.yan add
//        copyLink->setText(GENIE2_GET_TEXT(L_READY_SHARE_COPYLINK));

        if(lblAlarm)
        {
            lblAlarm->setText(GENIE2_GET_TEXT(L_READY_SHARE_TRYAGAIN ));

        }
    }

}

void QGenieRSBasic::slot_ProcessEnd(int ret)
{
    getFileExist(0==ret);
    qDebug()<<"slot_ProcessEnd:"<<ret;
    sRCFlags &= ~RCF_READYSHARE_PROCESS_ON;

}

//void QGenieRSBasic::BtnCopyLink()
//{
//    QClipboard *board = QApplication::clipboard();
//    QString link = QString("file:\\\\%1\\").arg(PCPForm::GetSingletonPtr()->Genie_IP());
//    board->setText(link);
//}
