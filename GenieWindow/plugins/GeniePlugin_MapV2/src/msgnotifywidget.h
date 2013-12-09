#ifndef MSGNOTIFYWIDGET_H
#define MSGNOTIFYWIDGET_H

#include <QWidget>
class QPushButton;
class QLabel;
class QTextEdit;
class QPropertyAnimation;

class msgnotifywidget : public QWidget
{
    Q_OBJECT
public:
    explicit msgnotifywidget(QWidget *parent = 0);

	void notifyNewDevice(QString msg, QString host);

protected:
	virtual void timerEvent(QTimerEvent *event);
	virtual void changeEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:

private slots:
    void okBtnClicked();

private:
	QPushButton         *m_okBtn;
	QPushButton         *m_closeBtn;
	QLabel              *m_titleLabel;
    QLabel              *m_titleContentLabel;
    QTextEdit           *m_bodyEdit;
	QPropertyAnimation  *m_notifyAnimation;

	int                 m_timeoutCounter;
	int                 m_timerId;
	bool                m_canMove;
    QPoint              m_pos;

	void retranslateUi();
    void applyStyle();
};

#endif // MSGNOTIFYWIDGET_H
