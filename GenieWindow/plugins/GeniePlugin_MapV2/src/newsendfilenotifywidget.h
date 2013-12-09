//add by caiyujun
#ifndef NEWSENDFILENOTIFYWIDGET_H
#define NEWSENDFILENOTIFYWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>
#include <Qdatetime>
#include <QProgressBar>
#include "api.h"
#include "udt.h"

class QPushButton;
class QLabel;
class QTextEdit;
class QPropertyAnimation;

class NewSendFileNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NewSendFileNotifyWidget(int iFolder, QWidget *parent = 0);

    ~NewSendFileNotifyWidget();
private:
    QPushButton         *m_closeBtn;
    QPushButton         *m_okBtn;
    QPushButton         *m_cancelBtn;
    //QPushButton         *m_rejectBtn;
    QLabel              *m_titleLabel;
    QLabel              *m_titleContentLabel;
    QTextEdit           *m_bodyEdit;
    QProgressBar        *m_progressBar;

    bool                m_canMove;
    QPoint              m_pos;
    QPropertyAnimation *m_notifyAnimation;
    int                 m_timeoutCounter;
    QString             m_strFileName;
    QString             m_strFile;

    int                 m_timerId;
protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void changeEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);


private:
    void retranslateUi();
    void applyStyle();
    QString translateText(int idx, const QString &defaultText = QString());

    //UDTSOCKET m_fhandle;
    int m_iFolder;
    //qint64 m_size;
    //bool m_sender;

signals:
    void translateText(int idx ,QString *text);
    void completed(int countRemain);
    void timeout();
    //void saveFile(QString strFilePath, int m_iFolder);
    //void rejectSaveFile(int iFolder);
    void cancelSend(); ///kai.yan
private slots:
    void btnClicked();
    //void okBtnClicked();
    void closeBtnClicked();
    void onSendProgress(qint64 iPercent, double speed);
    void onSendSuccess();
    void onReject();
    void onUnsupported();
    void slot_CancelBtnClicked();

public slots:
    void onFileChanged(QString strFile);
    void slot_sendFail(QString error);

public:
    void notifyNewDevice(const QString &title,int defaultCount = 0);

};

#endif // NEWSENDFILENOTIFYWIDGET_H
