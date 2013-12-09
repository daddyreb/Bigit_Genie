#include "stable.h"


QGenieDialogExit::QGenieDialogExit(QWidget *parent) :
    QDialog(parent),
    mDrag(false),
    ui(new Ui::QGenieDialogExit)
{
    ui->setupUi(this);
    this->setWindowFlags(DIALOG_FLAG| Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QGenieDialogExit::~QGenieDialogExit()
{
    delete ui;
}

void QGenieDialogExit::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}

void QGenieDialogExit::retranslateUi()
{
    DEFINE_LANGUAGE_MANAGER
    SET_TEXT(ui->but_ok,L_MAIN_OK);
    SET_TEXT(ui->but_cancel,L_MAIN_CANCEL);
    SET_TEXT(ui->label,L_MAIN_QUESTION_EXIT);
    SET_TEXT(ui->label_title,L_MAIN_EXIT);
    SET_TITLE(this,L_MAIN_EXIT);
}

void QGenieDialogExit::on_but_ok_clicked()
{
    qDebug()<<"QGenieDialogExit::on_but_ok_clicked()";
    mResult=ER_OK;
    this->hide();
}

void QGenieDialogExit::on_but_cancel_clicked()
{
    qDebug()<<"QGenieDialogExit::on_but_cancel_clicked()";
    mResult=ER_CANCEL;
    this->hide();
}

void QGenieDialogExit::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);

        event->accept();
    }
//    qDebug("mouse move");
}

void QGenieDialogExit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
      //  qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void	QGenieDialogExit::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
     //   qDebug("mouse release");
        mDrag=false;
        event->accept();
    }

}
