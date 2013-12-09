#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QWidget>

namespace Ui {
    class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(unsigned long ID, QWidget *parent = 0);
    ~OnlineUser();
	void SetText(QString userName, QString computerName, QString IP);
	unsigned long GetDlgID(){return m_ID;}
private:
	unsigned long m_ID;
	QString m_userName;
	QString m_computerName;
	QString m_IP;
private:
	void refurbishShow();
private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H
