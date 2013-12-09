#ifndef NEWDEVICENOTIFYWIDGET_H
#define NEWDEVICENOTIFYWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>


class QPushButton;
class QLabel;
class QTextEdit;
class QPropertyAnimation;

class NewDeviceNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NewDeviceNotifyWidget(QWidget *parent = 0);
    ~NewDeviceNotifyWidget();
private:
    QPushButton         *m_closeBtn;
    QPushButton         *m_okBtn;
    QLabel              *m_titleLabel;
    QLabel              *m_titleContentLabel;
    QTextEdit           *m_bodyEdit;

    bool                m_canMove;
    QPoint              m_pos;
    QPropertyAnimation *m_notifyAnimation;
    int                 m_timeoutCounter;
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

signals:
    void translateText(int idx ,QString *text);
    void completed(int countRemain);
    void timeout();
private slots:
    void btnClicked();

public:
    void notifyNewDevice(const QString &title,int defaultCount = 0);
};

#endif // NEWDEVICENOTIFYWIDGET_H
