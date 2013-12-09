//add by caiyujun

#include <Qdatetime>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QTimerEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>

#include "genie2_interface.h"

#include "newsendfilenotifywidget.h"
#include "language_Map.h"


#define NDNW_BACKGROUDIMAGE "map/others/NewDeviceNotifyWidgetBk.png"
#define NEWDEVICENOTIFYWIDGET_STYLEFILE "ui/newdevicenotifywidget.qss"
#define DELAYINTERVAL_MS 1000
#define DELAY_COUNTER_MAX 10

NewSendFileNotifyWidget::NewSendFileNotifyWidget(int iFolder, QWidget *parent) :
        QWidget(parent),
        m_canMove(false),
        m_notifyAnimation(0),
        m_timeoutCounter(DELAY_COUNTER_MAX),
        m_timerId(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    m_iFolder = iFolder;
    m_closeBtn = new QPushButton(this);
    m_closeBtn->setFlat(true);
    m_okBtn = new QPushButton(this);
    //m_rejectBtn = new QPushButton(this);
    //m_rejectBtn->setText("Reject");
//    SET_STYLE_PROPERTY(FLEX_BUTTON,m_okBtn);
    m_closeBtn->setObjectName("NewDeviceNotifyWidgetCloseButton");

    m_cancelBtn = new QPushButton(this);

    m_progressBar = new QProgressBar(this);

    m_titleLabel = new QLabel(this);
    m_titleContentLabel = new QLabel(this);
    m_bodyEdit = new QTextEdit(this);
    m_bodyEdit->setObjectName("NewDeviceNotifyWidgetEdit");
    m_bodyEdit->setEnabled(true);

    m_bodyEdit->setReadOnly(true);

    m_titleLabel->setObjectName("NewDeviceNotifyWidgetTitle");
    m_progressBar->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    m_titleContentLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_bodyEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QPixmap pixDetecter(GENIE2_RES(NDNW_BACKGROUDIMAGE) );
    setFixedSize(pixDetecter.size());

    QHBoxLayout *btnLayout = new QHBoxLayout();
#ifdef Q_OS_MAC
    btnLayout->setContentsMargins(0,2,0,0);
#endif
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_closeBtn);

    QHBoxLayout *okbtnLayout = new QHBoxLayout();
    okbtnLayout->setContentsMargins(0,0,0,0);
    okbtnLayout->addStretch(1);
    //okbtnLayout->addWidget(m_rejectBtn);
    okbtnLayout->addWidget(m_okBtn);
    okbtnLayout->addWidget(m_cancelBtn);

//    QHBoxLayout *rejectBtnLayout = new QHBoxLayout();
//    rejectBtnLayout->setContentsMargins(1,0,0,0);
//    rejectBtnLayout->addStretch(2);
//    rejectBtnLayout->addWidget(m_rejectBtn);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(20,0,20,0);
    //titleLayout->addWidget(m_progressBar);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addWidget(m_titleContentLabel);

    QVBoxLayout *aboveLayout = new QVBoxLayout();
#ifdef Q_OS_MAC
    aboveLayout->setContentsMargins(0,0,0,0);
#endif
    aboveLayout->setSpacing(9);
    aboveLayout->addLayout(btnLayout);
    aboveLayout->addLayout(titleLayout);

    QVBoxLayout *belowLayout = new QVBoxLayout();
    belowLayout->setContentsMargins(20,6,20,6);
    belowLayout->addWidget(m_bodyEdit);
    belowLayout->addWidget(m_progressBar);
    belowLayout->addLayout(okbtnLayout);

    QVBoxLayout *layOut = new QVBoxLayout(this);
#ifdef Q_OS_MAC
    layOut->setContentsMargins(9,2,4,2);
#else
    layOut->setContentsMargins(9,12,9,2);
#endif
    layOut->addLayout(aboveLayout);
    layOut->addLayout(belowLayout);

    connect(m_closeBtn,SIGNAL(clicked()),this,SLOT(closeBtnClicked()));
    connect(m_okBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    m_okBtn->setEnabled(false);
    //connect(m_rejectBtn,SIGNAL(clicked()),this,SLOT(btnClicked()));
    connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(slot_CancelBtnClicked()));

    applyStyle();
    retranslateUi();

    //m_fhandle = fhandle;
    //m_size = size;

    //m_sender = false;
}


NewSendFileNotifyWidget::~NewSendFileNotifyWidget()
{
//    if(m_timerId != 0)
//    {
//        killTimer(m_timerId);
//    }
}

void NewSendFileNotifyWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != m_timerId)
    {
        return;
    }

    if(--m_timeoutCounter <= 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        close();
    }
    else
    {
        retranslateUi();
    }
}

void NewSendFileNotifyWidget::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void NewSendFileNotifyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(GENIE2_RES(NDNW_BACKGROUDIMAGE)));
}

void NewSendFileNotifyWidget::retranslateUi()
{

    QString tmpText = translateText(L_MAP_YES,QString("OK"));
    if(m_timeoutCounter > 0)
    {
        tmpText += "(";
        tmpText += QString::number(m_timeoutCounter);
        tmpText += ")";
    }
    m_okBtn->setText(tmpText);
    m_cancelBtn->setText("Cancel");
////    if (m_sender)
////    {
    if (0 == m_timerId)
    {

//        if (m_iFolder == 0)
//        {
//            m_titleLabel->setText(translateText(L_MAP_NEWDEVICE,QString("Send file")));
//        }
//        else
//        {
//            m_titleLabel->setText(translateText(L_MAP_NEWDEVICE,QString("Send folder")));
//        }
    }
////    }
////    else
////    {

//        m_okBtn->setText(tmpText);
//        if (m_iFolder == 0)
//        {
//            m_titleLabel->setText(translateText(L_MAP_NEWDEVICE,QString("Send you a file")));
//        }
//        else
//        {
//            m_titleLabel->setText(translateText(L_MAP_NEWDEVICE,QString("Send you a folder")));
//        }

    //}

    //m_bodyEdit->setText(translateText(L_MAP_NEWDEVICENOTIFY_MSG,QString("Default content too long to place here")));
}

void NewSendFileNotifyWidget::applyStyle()
{
    QFile cssFile(GENIE2_RES(NEWDEVICENOTIFYWIDGET_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
    }
}

QString NewSendFileNotifyWidget::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void NewSendFileNotifyWidget::mousePressEvent(QMouseEvent *event)
{
    m_pos = event->globalPos();
    m_canMove = true;
}

void NewSendFileNotifyWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_canMove = false;
}

void NewSendFileNotifyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_canMove && (event->buttons() & Qt::LeftButton))
    {
        move(pos().x() - (m_pos.x() - event->globalPos().x()),
             pos().y() - (m_pos.y() - event->globalPos().y()));

        m_pos = event->globalPos();
    }
}



void NewSendFileNotifyWidget::btnClicked()
{
    if(m_timerId)
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
    close();
}

void NewSendFileNotifyWidget::slot_CancelBtnClicked()
{
    m_okBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
    m_titleLabel->setText("Undo Send");
    if (0 == m_timerId)
    {
        m_timerId = startTimer(DELAYINTERVAL_MS);
    }
    emit cancelSend();
}
void NewSendFileNotifyWidget::closeBtnClicked()
{
    close();
}



void NewSendFileNotifyWidget::onSendProgress(qint64 iPercent, double speed)
{
//    qint64 a = (iProgress*100/iSize);
//    if(a ==1) {
//            a = 1;
//    }
//    if(a > 100)
//    {
//            a = a;
//    }
//    char sendPercent[100];
//    memset(sendPercent, 0, 100);
//    sprintf(sendPercent, "%I64d%%", a);
    m_progressBar->setValue(iPercent);
    qDebug() << "file widget progress";
    if (0 != iPercent && 0.009 < speed)
    {
        QString strSpeed = QString::number(speed);
        QString strSpilt = strSpeed.left(4);
        strSpilt += "MB/sec";
        m_titleLabel->setText(strSpilt);
    }
}

void NewSendFileNotifyWidget::onSendSuccess()
{
    //m_rejectBtn->setEnabled(true);
    m_okBtn->setEnabled(true);
    m_cancelBtn->setVisible(false);
    if (0 == m_timerId)
    {
        m_timerId = startTimer(DELAYINTERVAL_MS);
    }
    m_titleLabel->setText(tr("Send Success"));
}

void NewSendFileNotifyWidget::onReject()
{
    qDebug() << "receive reject rsp";
    m_okBtn->setEnabled(true);
    m_cancelBtn->setVisible(false);
    if (0 == m_timerId)
    {
        m_timerId = startTimer(DELAYINTERVAL_MS);
    }
    m_titleLabel->setText(tr("Remote Rejected"));
}

void NewSendFileNotifyWidget::onUnsupported()
{
    qDebug() << "mobile device unsupported";
    m_okBtn->setEnabled(true);
    m_cancelBtn->setVisible(false);
    if (0 == m_timerId)
    {
        m_timerId = startTimer(DELAYINTERVAL_MS);
    }
    m_titleLabel->setText(tr("Mobile Device Unsupported"));
}

void NewSendFileNotifyWidget::onFileChanged(QString strFile)
{
    qDebug() << "file changed";
    m_bodyEdit->setText(strFile);
}


void NewSendFileNotifyWidget::notifyNewDevice(const QString &title,int defaultCount)
{
    //m_titleContentLabel->setText(title);

//    QMessageBox msgbox;
//    msgbox.setText(title);
//    msgbox.exec();
    m_strFileName = title;
//    QMessageBox msgbox;
//    msgbox.setText(title);
//    msgbox.exec();

    QString strText;
    if (0 == m_iFolder)
    {
        strText += "Send file ";
        strText += title;
        m_bodyEdit->setText(strText);
    }
    else if (1 == m_iFolder)
    {
        strText += "Send folder ";
        strText += title;
        m_bodyEdit->setText(strText);
    }
    else if (2 == m_iFolder)
    {
        strText += "Send file ";
        strText += title;
        strText += " and other ";
        strText += QString::number(defaultCount);
        strText += " files";
        m_bodyEdit->setText(strText);
    }


    //m_bodyEdit->setText(m_strFileName);
    setVisible(true);
    QRect rect = QApplication::desktop()->screenGeometry(-1);
    int x = rect.width() - width() - 10;

    if(!m_notifyAnimation)
    {
        m_notifyAnimation = new QPropertyAnimation(this, "geometry",this);
    }

    m_notifyAnimation->setDuration(1000);
    m_notifyAnimation->setStartValue(QRect(x, rect.height(), width(), height()));
    m_notifyAnimation->setEndValue(QRect(x, rect.height() - height() - 40, width(), height()));
    m_notifyAnimation->start();


//    QString strFileName = "a.txt";
//    QStringList strList = strFileName.split(".");
//    QString strSuffix;
//    QString strFilter;
//    if (!strList.empty())
//    {
//        int iLen = strList.length();
//        strSuffix = strList.at(iLen-1);
//        strFilter += strSuffix;
//        strFilter += " files(*.";
//        strFilter += strSuffix;
//        strFilter += ")";
//    }
//    else
//    {
//        strFilter += "All Files(*.*)";
//    }
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//                               strFileName,
//                               strFilter);

//    QMessageBox msgbox;
//    msgbox.setText(fileName);
//    msgbox.exec();
    //timer
    if(m_timerId != 0)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        m_timeoutCounter = (defaultCount <= 0 ? DELAY_COUNTER_MAX : defaultCount);
    }

}
void NewSendFileNotifyWidget::slot_sendFail(QString error)
{
    m_titleLabel->setText(error);
    m_okBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
    if (0 == m_timerId)
    {
        m_timerId = startTimer(DELAYINTERVAL_MS);
    }
}
