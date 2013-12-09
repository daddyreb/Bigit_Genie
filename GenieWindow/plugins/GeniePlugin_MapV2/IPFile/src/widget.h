#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include "msgmng.h"
#include <WinSock2.h>
#include <QMap>
#include <QList>
#include <QUrl>
#include "onlineuser.h"
#include "userdlg.h"
#include "filethread.h"

namespace Ui {
    class Widget;
}

struct User{
	unsigned long IP;
	QString userName;
	QString computerName;
	int row;
	QWidget* dlg;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

	private slots:
		void ErrorProcessrSlot(QString errString);
		void RecvMsgSlot(MsgBuf *msg);
		void SendMessageErrorSLot(int err);
		void DoubleClickedSlot(int row, int column);
		void UserSendFileSlot(qint64 fileID);
		void UserCancelSendFileSlot(qint64 fileID);
		void UserDlgCloseSlot(unsigned long ID);
		void UserRefuseRecvFileSlot(qint64 fileID);
		void UserCancelRecvFileSlot(qint64 fileID);
private:
		MsgMng *m_msgThread;
		CFileThread *m_fileThread;
		bool m_error;
		SOCKET m_msgSocket;
		QMap<unsigned long, User> m_onlineUserMap;
private:
	void UiInit();
        void LoadFileStatus();
	bool GetError() {return m_error;}
	void BroadcastEntry();
	void BroadcastExit();
	void BroadcastMsg(unsigned long command, char* msg = NULL);
	bool GetBroadcastAddr();
	void MsgBroadcastEntry(MsgBuf *msg);
	void MsgOnline(MsgBuf *msg);
	void MsgBroadcastExit(MsgBuf *msg);
	void MsgSendFile(MsgBuf *msg);
	void MsgRefuseRecvFile(MsgBuf *msg);
	void MsgCancelSendFile(MsgBuf *msg);
	void MsgCancelRecvFile(MsgBuf *msg);

	void AddUserList(MsgBuf *msg);
	void DeleteUserList(MsgBuf *msg);
	int OnlineUserNumber();
};

#endif // WIDGET_H
