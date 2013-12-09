#include "QGenieMsgDialog.h"
#include "ui_QGenieMsgDialog.h"
#include <QDebug>
#include <QMouseEvent>
#include "ILanguageManager.h"
#include "IManagerHandler.h"
#include "mainwindow.h"
#include "common.h"

#ifdef SHEET_STYLE_FORMAC
#define DIALOG_FLAG (Qt::Sheet| Qt::FramelessWindowHint)
#else
#define DIALOG_FLAG (Qt::Tool| Qt::FramelessWindowHint)
#endif

QGenieMsgDialog::QGenieMsgDialog(QWidget *parent) :
    QDialog(parent),
#ifndef SHEET_STYLE_FORMAC
    mDrag(false),
#endif
    ui(new Ui::QGenieMsgDialog),
    mTitle(-1),
    mMsg(-1)
{
    ui->setupUi(this);
    this->setWindowFlags(DIALOG_FLAG| Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}



QGenieMsgDialog::~QGenieMsgDialog()
{
    delete ui;
}

void QGenieMsgDialog::showMsg(int title,int msg)
{
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    QString stitle=lanmanager->getText(title,QUuid(PLUGIN_UUID));
    QString smsg=lanmanager->getText(msg,QUuid(PLUGIN_UUID));
    mTitle=title;
    mMsg=msg;
    showMsg(stitle,smsg);
}

void QGenieMsgDialog::showMsg(QString title,QString msg)
{
    ui->label_title->setText(title);
    ui->label->setText(msg);
#ifdef SHEET_STYLE_FORMAC
    show();
    qDebug()<<"QGenieMsgDialog::showMsg";
#else
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
        ui->label_title->setText(lanmanager->getText(mTitle,QUuid(PLUGIN_UUID)));
    }
    else if(mMsg >=0)
    {
        ui->label->setText(lanmanager->getText(mMsg,QUuid(PLUGIN_UUID)));
    }
    ui->but_ok->setText(lanmanager->getText(L_TRAFFICMETER_IMSGBOX_OK,QUuid(PLUGIN_UUID)));
    ui->but_cancel->setText(lanmanager->getText(L_TRAFFICMETER_IMSGBOX_CANCEL,QUuid(PLUGIN_UUID)));
}

void QGenieMsgDialog::on_but_ok_clicked()
{
    qDebug()<<"QGenieMsgDialog::on_but_ok_clicked()";
    mResult=ER_OK;
    this->hide();
}

void QGenieMsgDialog::on_but_cancel_clicked()
{
    qDebug()<<"QGenieMsgDialog::on_but_cancel_clicked()";
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
