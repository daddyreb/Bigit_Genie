#ifndef USERDLG_H
#define USERDLG_H

#include <QWidget>
#include <QDropEvent>
#include <QUrl>
#include "IPFileMsg.h"
#include <Windows.h>
#include <QMap>
#include <QThread>

namespace Ui {
    class UserDlg;
}

class UserDlg : public QWidget
{
    Q_OBJECT

public:
    explicit UserDlg(unsigned long ID, QWidget *parent = 0);
	explicit UserDlg(unsigned long ID, qint64 fileID, QString fileName, QWidget *parent = 0);
    ~UserDlg();

private:
    Ui::UserDlg *ui;
	unsigned long m_ID;
	QMap<qint64, QThread*> m_countFileThreadMap;
protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	public slots:
		void RefuseRecvFileSlot();
		void FileSizeSlot(qint64 fileID, qint64 fileSize, QList<QString> fileList, QString fileCheck);
		void CancelSlot(qint64);
		void CancelRecvSlot(qint64);
signals:
	void UserSendFileSignal(qint64 fileID);
	void UserSendMsgSignal(QList<QString> msg);
	void UserDlgCloseSignal(unsigned long ID);
	void UserCancelSendFileSignal(qint64 fileID);
	void UserRefuseRecvFileSignal(qint64 fileID);
	void UserCancelRecvFileSignal(qint64 fileID);
public:
	unsigned long GetUserID();
	void SetRecvFile(qint64 fileID, QString fileName, qint64 fileSize, int fileType, QString fileCheck);
	void CancelRecvFile(qint64 fileID);
	void RefuseRecvFile(qint64 fileID);
	void CancelSendFile(qint64 fileID);
private:
	qint64 TimeNo(void);
};

#endif // USERDLG_H
