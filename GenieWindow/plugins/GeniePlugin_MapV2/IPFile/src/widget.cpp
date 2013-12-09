#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QMessageBox>
#include <QList>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QSettings>

Widget *mainWidget;
SOCKET msgSocket;

QMap<qint64, FileStatus> recvFileStatusMap;
QMutex recvFileStatusMapMutex;

QMap<qint64, FileStatus> sendFileStatusMap;
QMutex sendFileStatusMapMutex;

QMap<QString, RecvStatus> recvStatusControlMap;
QMutex recvStatusControlMapMutex;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
	m_error = false;

	mainWidget = this;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_msgThread = new MsgMng(m_msgSocket, IPFILE_PORT);
	msgSocket = m_msgSocket;
	connect(m_msgThread, SIGNAL(ErrorOccurSignal(QString)), this, SLOT(ErrorProcessrSlot(QString)));
	connect(m_msgThread, SIGNAL(RecvMsgSignal(MsgBuf*)), this, SLOT(RecvMsgSlot(MsgBuf*)));
	connect(m_msgThread, SIGNAL(SendMsgErrorSignal(int)), this, SLOT(SendMessageErrorSLot(int)));
	connect(ui->userTableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(DoubleClickedSlot(int, int)));
	m_msgThread->start();

	m_fileThread = new CFileThread(IPFILE_PORT);
	connect(m_fileThread, SIGNAL(ErrorOccurSignal(QString)), this, SLOT(ErrorProcessrSlot(QString)));
	m_fileThread->start();

	UiInit();

	LoadFileStatus();
	BroadcastEntry();
	setAttribute(Qt::WA_DeleteOnClose);	
}

Widget::~Widget()
{	
	delete ui;
	m_msgThread->Stop();
	m_msgThread->wait();
	m_fileThread->Stop();
	m_fileThread->wait();
	BroadcastExit();
	QMap<unsigned long, User>::iterator iter;
	for(iter=m_onlineUserMap.begin(); iter!=m_onlineUserMap.end(); iter++) {
		if(iter->dlg) {
			delete iter->dlg;
		}
	}
}

void Widget::UiInit()
{
	ui->titleLabel->setAlignment(Qt::AlignCenter);
	ui->userTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//固定大小
	ui->userTableWidget->verticalHeader()->setVisible(false);//不显示横向标题
	ui->userTableWidget->horizontalHeader()->setVisible(false);
	ui->userTableWidget->setSelectionBehavior(QTableWidget::SelectRows);//一次性先选中一行
	ui->userTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许编辑表格
}

void Widget::LoadFileStatus()
{
	QSettings *fileStatus = new QSettings(IPFILE_STATUSFILE,  QSettings::IniFormat);
	fileStatus->setIniCodec("UTF-8");
	QStringList keys = fileStatus->childGroups();
	QStringList::iterator iter;
	for(iter=keys.begin(); iter!=keys.end(); iter++) {
		RecvStatus recvStatus;
		QString key = QString("%1/status").arg(*iter);
		QString path = QString("%1/path").arg(*iter);
		recvStatus.status = fileStatus->value(key).toInt();
		recvStatus.filePath =fileStatus->value(path).toString();

		recvStatusControlMapMutex.lock();
		recvStatusControlMap.insert(*iter, recvStatus);
		recvStatusControlMapMutex.unlock();
	}
	delete fileStatus;
}

void Widget::RecvMsgSlot(MsgBuf *msg)
{
	switch(GET_MODE(msg->command))
	{
	case IPFILE_BR_ENTER: //上线
		MsgBroadcastEntry(msg);
		break;
	case IPFILE_BR_EXIT: //推出
		MsgBroadcastExit(msg);
		break;
	case IPFILE_ONLINE://在线
		MsgOnline(msg);
		break;
	case IPFILE_SENDFILE://发送文件
		MsgSendFile(msg);
		break;
	case IPFILE_CANCELSENDFILE: //取消文件发送
		MsgCancelSendFile(msg);
		break;
	case IPFILE_REFUSERECVFILE: //拒绝接收文件
		MsgRefuseRecvFile(msg);
		break;
        case IPFILE_CONCELRECVFILE: //取消文件接收
		MsgCancelRecvFile(msg);
		break;
	}
	delete msg;
}

void Widget::ErrorProcessrSlot(QString errString)
{

}

void Widget::SendMessageErrorSLot(int err)
{
	
}

bool Widget::GetBroadcastAddr()
{
	QList<QNetworkInterface>list = QNetworkInterface::allInterfaces();//获取所有网络接口信息
	QList<QNetworkInterface>::iterator iter;
	for(iter=list.begin(); iter!=list.end(); iter++) {
		QList<QNetworkAddressEntry>entryList = iter->addressEntries();
		QList<QNetworkAddressEntry>::iterator i;
		for(i=entryList.begin(); i!=entryList.end(); i++) {
			i->broadcast().toString();
		}
	}
	return 0;
}

void Widget::BroadcastEntry()
{
	BroadcastMsg(IPFILE_BR_ENTER);
}

void Widget::BroadcastExit()
{
	BroadcastMsg(IPFILE_BR_EXIT);
}

void Widget::BroadcastMsg(unsigned long command, char* msg /* = NULL */)
{
	QList<QNetworkInterface>list = QNetworkInterface::allInterfaces();//获取所有网络接口信息
	QList<QNetworkInterface>::iterator iter;
	for(iter=list.begin(); iter!=list.end(); iter++) {
		QList<QNetworkAddressEntry>entryList = iter->addressEntries();
		QList<QNetworkAddressEntry>::iterator i;
		for(i=entryList.begin(); i!=entryList.end(); i++) {
			QString strAddress = i->broadcast().toString();
			unsigned long host_addr = inet_addr(strAddress.toLocal8Bit());
			m_msgThread->Send(host_addr, htons(IPFILE_PORT), command, msg);
		}
	}
}

void Widget::MsgBroadcastEntry(MsgBuf *msg)
{
	if(GET_MODE(msg->command) == IPFILE_BR_ENTER) {
		AddUserList(msg);
	}
	m_msgThread->Send(&(msg->hostSub), IPFILE_ONLINE);
	OnlineUserNumber();
}


void Widget::MsgOnline(MsgBuf *msg)
{
	if(GET_MODE(msg->command) == IPFILE_ONLINE) {
		AddUserList(msg);
	}
	OnlineUserNumber();
}

void Widget::MsgBroadcastExit(MsgBuf *msg)
{
	if(GET_MODE(msg->command) == IPFILE_BR_EXIT) {
		DeleteUserList(msg);
	}
	OnlineUserNumber();
}
 
void Widget::MsgSendFile(MsgBuf *msg)
{
	unsigned long userID = msg->hostSub.addr;
	if(!m_onlineUserMap.contains(userID)) {
		AddUserList(msg);
	}
	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(userID);
	char *szFileID = strtok(msg->msgBuf, "*");
	if(!szFileID) { 
		return;
	}
	qint64 fileID;
	sscanf(szFileID, "%I64d", &fileID);

	char *szFileName = strtok(NULL, "*");
	if(!szFileName) {
		return;
	}

	char *szFileSize = strtok(NULL, "*");
	if(!szFileSize) {
		return;
	}
	qint64 fileSize;
	sscanf(szFileSize, "%I64d", &fileSize);
	
	char *szFileType = strtok(NULL, "*");
	if(!szFileType) {
		return;
	}
	int fileType;
	sscanf(szFileType, "%d", &fileType);

	char *szFileCheck = strtok(NULL, "*");
	if(!szFileCheck) {
		return;
	}

	if(iter->dlg == NULL) {
		iter->dlg = new UserDlg(userID);
	}
	((UserDlg*)(iter->dlg))->SetRecvFile(fileID, QString::fromLocal8Bit(szFileName), fileSize, fileType, szFileCheck);
	iter->dlg->show();
	iter->dlg->activateWindow();
}

void Widget::MsgCancelSendFile(MsgBuf *msg)
{
	unsigned long userID = msg->hostSub.addr;
	if(!m_onlineUserMap.contains(userID)) {
		AddUserList(msg);
	}
	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(userID);
	qint64 fileID;
	sscanf(msg->msgBuf, "%I64d", &fileID);
	if(iter->dlg == NULL) {
		return;
	}
	((UserDlg*)(iter->dlg))->CancelRecvFile(fileID);
}

void Widget::MsgCancelRecvFile(MsgBuf *msg)
{
	unsigned long userID = msg->hostSub.addr;
	if(!m_onlineUserMap.contains(userID)) {
		AddUserList(msg);
	}
	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(userID);
	qint64 fileID;
	sscanf(msg->msgBuf, "%I64d", &fileID);
	if(iter->dlg == NULL) {
		return;
	}
	((UserDlg*)(iter->dlg))->CancelSendFile(fileID);
}

void Widget::MsgRefuseRecvFile(MsgBuf *msg)
{
	unsigned long userID = msg->hostSub.addr;
	qint64 fileID ;
	sscanf(msg->msgBuf, "%I64d", &fileID);
	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(userID);
	UserDlg *userDlg = (UserDlg*)iter->dlg;
	if(userDlg) {
		userDlg->show();
		userDlg->activateWindow();
		userDlg->RefuseRecvFile(fileID);//处理对方拒接接收文件的消息
	}
}

void Widget::AddUserList(MsgBuf *msg)
{
	User user;
	if(msg->hostSub.hostName[0] != 0) {
		user.computerName = QString::fromLocal8Bit(msg->hostSub.hostName);
	}

	if(msg->hostSub.userName[0] != 0) {
		user.userName = QString::fromLocal8Bit(msg->hostSub.userName);
	}

	if(msg->hostSub.addr == 0) {
		return;
	}
	user.IP = msg->hostSub.addr;

	if(m_onlineUserMap.contains(user.IP)) {
		return ;
	}
	
	in_addr addr;
	addr.S_un.S_addr = msg->hostSub.addr;
	QString IP = QString::fromLocal8Bit(inet_ntoa(addr));

	OnlineUser *userItem = new OnlineUser(user.IP, this);
	userItem->SetText(user.userName, user.computerName, IP);
	userItem->setStyleSheet("background-color:red");

	int row = ui->userTableWidget->rowCount();
	ui->userTableWidget->insertRow(row);
	ui->userTableWidget->setCellWidget(row, 0, userItem);
	ui->userTableWidget->resizeColumnsToContents();
	ui->userTableWidget->resizeRowsToContents();

	//userWidget->show();
	user.row = row;
	user.dlg = NULL;
	m_onlineUserMap.insert(user.IP, user);
}

void Widget::DeleteUserList(MsgBuf *msg)
{
	if(msg->hostSub.addr == 0) {
		return;
	}
	unsigned long ID = msg->hostSub.addr;

	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(ID);
	if(m_onlineUserMap.end() != iter) {
		int row = ui->userTableWidget->rowCount();
		for(int i=0; i<row; i++) {
			OnlineUser *userItem = (OnlineUser*)ui->userTableWidget->cellWidget(i, 0);
			if(userItem) {
				if(userItem->GetDlgID() == ID) {
					delete userItem;
					ui->userTableWidget->removeRow(i);
					break;
				}
			}
		}
	}
	m_onlineUserMap.erase(iter);
}

int Widget::OnlineUserNumber()
{
	int row = ui->userTableWidget->rowCount();
	ui->titleLabel->setText(QString::fromLocal8Bit("在线用户数 %1").arg(row));
	return 0;
}

void Widget::DoubleClickedSlot(int row, int column)
{
	OnlineUser *userDlg = (OnlineUser *)ui->userTableWidget->cellWidget(row, column);
	if(userDlg) {
		unsigned long ID = userDlg->GetDlgID();
		QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(ID);
		if((iter != m_onlineUserMap.end()) && (iter->IP == ID)) {
			if(iter->dlg == NULL) {
				iter->dlg = new UserDlg(ID);
			}
			iter->dlg->show();
			iter->dlg->activateWindow();
		}
	}
}

void Widget::UserDlgCloseSlot(unsigned long ID)
{
	QMap<unsigned long, User>::iterator iter = m_onlineUserMap.find(ID);
	if((iter != m_onlineUserMap.end()) && (iter->IP == ID)) {
		if(iter->dlg) {
			iter->dlg = NULL;
		}
	}
}

void Widget::UserSendFileSlot(qint64 fileID)
{
	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter;
	iter = sendFileStatusMap.find(fileID);
	if(iter == sendFileStatusMap.end()) {
		sendFileStatusMapMutex.unlock();
		return;
	}
	char fileInfo[4096] = {0};
	sprintf(fileInfo, "%I64d*%s*%I64d*%d*%s*", iter->fileID, iter->fileName.toLocal8Bit().data(), iter->fileSize, iter->fileType, iter->fileCheck.toAscii().data());
	m_msgThread->Send(iter->ip, htons(IPFILE_PORT), IPFILE_SENDFILE, fileInfo);
	sendFileStatusMapMutex.unlock();
	//for(iter=fileList.begin(); iter!=fileList.end(); iter++) {
	//	iter->toLocalFile();
	//}
}

void Widget::UserCancelSendFileSlot(qint64 fileID)
{
	unsigned long userID = 0;
	UserDlg *userDlg = (UserDlg*)sender();
	userID = userDlg->GetUserID();
	char szFileID[1024] = {0};
	sprintf(szFileID, "%I64d", fileID);
	m_msgThread->Send(userID, htons(IPFILE_PORT), IPFILE_CANCELSENDFILE, szFileID);
}

void Widget::UserRefuseRecvFileSlot(qint64 fileID)
{
	unsigned long userID = 0;
	UserDlg *userDlg = (UserDlg*)sender();
	userID = userDlg->GetUserID();
	char szFileID[1024] = {0};
	sprintf(szFileID, "%I64d", fileID);
	m_msgThread->Send(userID, htons(IPFILE_PORT), IPFILE_REFUSERECVFILE, szFileID);
}

void Widget::UserCancelRecvFileSlot(qint64 fileID)
{
	unsigned long userID = 0;
	UserDlg *userDlg = (UserDlg*)sender();
	userID = userDlg->GetUserID();
	char szFileID[1024] = {0};
	sprintf(szFileID, "%I64d", fileID);
	m_msgThread->Send(userID, htons(IPFILE_PORT), IPFILE_CONCELRECVFILE, szFileID);
}
