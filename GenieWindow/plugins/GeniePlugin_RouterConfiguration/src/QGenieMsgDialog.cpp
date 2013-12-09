#include "QGenieMsgDialog.h"
#include "ui_QGenieMsgDialog.h"
#include <QDebug>
#include <QMouseEvent>
#include "ILanguageManager.h"
#include "IManagerHandler.h"
#include "mainwindow.h"
#include "LanguageRouterConfiguration.h"

#ifdef SHEET_STYLE_FORMAC
#define DIALOG_FLAG (Qt::Sheet| Qt::FramelessWindowHint)
#else
#define DIALOG_FLAG (Qt::Tool| Qt::FramelessWindowHint)
#endif

#define GACC_DISABLE_DLG_DURATION (12*1000)
#define GACC_DISABLE_DLG_FRAME_COUNT (10*1000)

QGenieMsgDialog::QGenieMsgDialog(QWidget *parent) :
    QDialog(parent),
#ifdef PLATFORM_WIN32
    mDrag(false),
#endif
    ui(new Ui::QGenieMsgDialog),
    mTitle(-1),
    mMsg(-1),
    mbUseTimeLine(false)
{
    ui->setupUi(this);

    this->setWindowFlags(DIALOG_FLAG| Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::ApplicationModal);

    this->setAttribute(Qt::WA_TranslucentBackground);
    connect(&mTimeLine,SIGNAL(finished()),this,SLOT(hide()));
    connect(&mTimeLine,SIGNAL(frameChanged(int)),this,SLOT(slot_timelineUpdate(int)));
}



QGenieMsgDialog::~QGenieMsgDialog()
{
    delete ui;
}

void QGenieMsgDialog::showMsg(int title,int msg,bool usetimeline)
{
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    QString stitle=lanmanager->getText(title,QUuid(UUID_OF_THIS));
    QString stext=lanmanager->getText(msg,QUuid(UUID_OF_THIS));
    mTextRemain="";
    mTitle=title;
    mMsg=msg;
    showMsg(stitle,stext,usetimeline);
}

void QGenieMsgDialog::showMsg(QString title,QString msg,bool usetimeline)
{
    mResult=ER_CANCEL;
    ui->label_title->setText(title);
    ui->label->setText(msg);
    mbUseTimeLine=usetimeline;
    mText=msg;
    mResult=ER_CANCEL;

    if(usetimeline)
    {

        mTimeLine.setDuration(GACC_DISABLE_DLG_DURATION);
        mTimeLine.setFrameRange(0,GACC_DISABLE_DLG_FRAME_COUNT);
        mTimeLine.setCurveShape(QTimeLine::LinearCurve);
        mTimeLine.setUpdateInterval(400);
        ui->label->setText(QString("%1(%2)").arg(msg,QString::number(GACC_DISABLE_DLG_FRAME_COUNT/1000)));

        mTimeLine.setStartFrame(0);
        mTimeLine.setDirection(QTimeLine::Forward);
        mTimeLine.start();
    }
    IUiManager *uimanager=MainWindow::sHandler->getUiManager();
    Q_ASSERT(uimanager);
    uimanager->command(UI_CMD_SHOWNORMAL,"");
#ifdef SHEET_STYLE_FORMAC
    show();
#else
	hide();
	QTimer::singleShot(0,this,SLOT(show()));
	exec();
#endif
}

void QGenieMsgDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}

void QGenieMsgDialog::retranslateUi()
{
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    if(mTitle >=0)
    {
        ui->label_title->setText(lanmanager->getText(mTitle,QUuid(UUID_OF_THIS)));
    }
    else if(mMsg >=0)
    {
        mText=lanmanager->getText(mMsg,QUuid(UUID_OF_THIS));
        if(!mbUseTimeLine)
        {
            ui->label->setText(mText);
        }
        else
        {
            int remain=(mTimeLine.state()!=QTimeLine::Running)?0
                      :((mTimeLine.endFrame()-mTimeLine.currentFrame())/1000+1);
            ui->label->setText(QString("%1(%2)").arg(mText,QString::number(remain)));
        }
    }
    ui->but_ok->setText(lanmanager->getText(L_RTCF_IMSGBOX_OK,QUuid(UUID_OF_THIS)));
    ui->but_cancel->setText(lanmanager->getText(L_RTCF_CANCEL,QUuid(UUID_OF_THIS)));
}

void QGenieMsgDialog::on_but_ok_clicked()
{
    qDebug()<<"QGenieMsgDialog::on_but_ok_clicked()";
    mTimeLine.stop();
    mResult=ER_OK;
    this->hide();
}

void QGenieMsgDialog::on_but_cancel_clicked()
{
    qDebug()<<"QGenieMsgDialog::on_but_cancel_clicked()";
    mTimeLine.stop();
    mResult=ER_CANCEL;
    this->hide();
}
#ifndef SHEET_STYLE_FORMAC
void QGenieMsgDialog::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);

        event->accept();
    }
//    qDebug("mouse move");
}

void QGenieMsgDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
      //  qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void	QGenieMsgDialog::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
     //   qDebug("mouse release");
        mDrag=false;
        event->accept();
    }

}
#endif

void QGenieMsgDialog::slot_timelineUpdate(int c)
{
    qDebug()<<mTimeLine.duration()<<c;
    int remain=(mTimeLine.endFrame()-c)/1000+1;
    ui->label->setText(QString("%1(%2)").arg(mText,QString::number(remain)));

}
