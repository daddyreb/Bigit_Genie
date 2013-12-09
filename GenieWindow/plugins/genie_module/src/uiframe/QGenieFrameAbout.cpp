#include "stable.h"



QGenieFrameAbout::QGenieFrameAbout(QWidget *parent) :
    QDialog(parent)
    ,ui(new Ui::QGenieFrameAbout)
    ,mDrag(false)
{ 
    ui->setupUi(this);
    setWindowTitle(tr("About"));
    setWindowIcon(QIcon(":/ico_tray_good.ico"));
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    //ui->line->setFixedHeight(2);
    this->setWindowFlags(DIALOG_FLAG);
    this->setAttribute(Qt::WA_TranslucentBackground);

  /*  QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(frameRect());
    QRect rect = boardRect();
    painter.fillRect(rect, Qt::white);
*/
 //   setStyleSheet("background-color:white;");
  //  setAttribute( Qt::WA_DeleteOnClose, true );
}

QGenieFrameAbout::~QGenieFrameAbout()
{
    delete ui;
}

void QGenieFrameAbout::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    if (e->type()==QEvent::LanguageChange)
    {
        DEFINE_LANGUAGE_MANAGER
        SET_TEXT(ui->label,L_MAIN_FRAME_ABOUT_TITLE);
        SET_TEXT(ui->close_button,L_MAIN_FRAME_CLOSE);
        SET_TITLE(this,L_MAIN_FRAME_ABOUT_TITLE);
    }
}
#ifndef SHEET_STYLE_FORMAC
void QGenieFrameAbout::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);

        event->accept();
    }
//    qDebug("mouse move");
}

void QGenieFrameAbout::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
      //  qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void	QGenieFrameAbout::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
     //   qDebug("mouse release");
        mDrag=false;
        event->accept();
    }

}
#endif

