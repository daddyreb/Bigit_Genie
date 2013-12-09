//#include "stable.h"
#include "QGenieSelfTrayMessage.h"
#ifdef Q_OS_MACX
#include "ui_QGenieSelfTrayMessage_Mac.h"
#else
#include "ui_QGenieSelfTrayMessage_Windows.h"
#endif
QGenieSelfTrayMessage::QGenieSelfTrayMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieSelfTrayMessage)
{
    ui->setupUi(this);
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(close()));
#ifdef Q_OS_MACX
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
#else
    this->setWindowFlags(Qt::Tool| Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
#endif
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QGenieSelfTrayMessage::~QGenieSelfTrayMessage()
{
    mTimer.stop();
    delete ui;
}

void QGenieSelfTrayMessage::showText(const QString &txt,int secs)
{
    ui->label_txt->setText(txt);
    show();
    mTimer.stop();
    mTimer.setSingleShot(true);
    mTimer.start(1000*secs);

}

void QGenieSelfTrayMessage::setPosition(int x, int y)
{
    QDesktopWidget* desktop = QApplication::desktop();
    int screenIndex=desktop->screenNumber(QPoint(x,y));
    QRect rect=desktop->screenGeometry(screenIndex);
    const int posidx=((y<(rect.height()/2))?0:1)*2
            +((x<(rect.width()/2))?0:1);

    qDebug()<<"tray idx ="<<posidx;
    const char *cImageName[]={
        "selftray_msg_lu.png",
        "selftray_msg_ru.png",
        "selftray_msg_ld.png",
        "selftray_msg_rd.png"
    };
    QString stylesheet="QFrame#frame{"
            "background-image: url(:/images/self_tray/%1);"
    "}";
    stylesheet=stylesheet.arg(cImageName[posidx]);
    ui->frame->setStyleSheet(stylesheet);

    const int cXOffsets[]={-16,-117,-16,-116};
    const int cYOffsets[]={-0,-2,-92,-92};

    const int cTitleY[]={30,30,11,11};
    const int cTxtY[]={60,60,39,39};
    QRect rtitle=ui->label_title->geometry();
    rtitle.moveTop(cTitleY[posidx]);
    ui->label_title->setGeometry(rtitle);
    QRect rtext=ui->label_txt->geometry();
    rtext.moveTop(cTxtY[posidx]);
    ui->label_txt->setGeometry(rtext);

    this->move(x+cXOffsets[posidx],y+cYOffsets[posidx]);


}



void	QGenieSelfTrayMessage::mousePressEvent ( QMouseEvent * event )
{
    QWidget::mousePressEvent(event);
//    QGenieTrayManager *trayManager=QGenieTrayManager::GetSingletonPtr();
//    if(trayManager)
//    {
        close();
//    }
}




