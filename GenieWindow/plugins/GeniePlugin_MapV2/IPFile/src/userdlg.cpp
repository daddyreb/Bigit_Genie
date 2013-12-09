#include "userdlg.h"
#include "ui_userdlg.h"
#include <QEvent>
#include <QUrl>
#include "filetranstatus.h"
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMap>
#include "IPFileMsg.h"
#include <QTableWidgetItem>
#include "filecount.h"
#include "widget.h"
#include "filetranthread.h"
#include "recvthread.h"

extern Widget *mainWidget;
extern SOCKET msgSOKCET;

extern QMap<qint64, FileStatus> recvFileStatusMap;
extern QMutex recvFileStatusMapMutex;

extern QMap<qint64, FileStatus> sendFileStatusMap;
extern QMutex sendFileStatusMapMutex;

extern QMap<QString, RecvStatus> recvStatusControlMap;
extern QMutex recvStatusControlMapMutex;

UserDlg::UserDlg(unsigned long ID, QWidget *parent) :
QWidget(parent),
ui(new Ui::UserDlg)
{
	m_ID = ID;
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setAcceptDrops(true);
	in_addr in;
	in.S_un.S_addr = m_ID;
	QString caption = QString("%1").arg(QString::fromLocal8Bit(inet_ntoa(in)));
	setWindowTitle(caption);
	ui->fileTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//固定大小
	ui->fileTableWidget->verticalHeader()->setVisible(false);//不显示横向标题
	ui->fileTableWidget->horizontalHeader()->setVisible(false);
	ui->fileTableWidget->setSelectionBehavior(QTableWidget::SelectRows);//一次性先选中一行
	ui->fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许编辑表格

	connect(this, SIGNAL(UserSendFileSignal(qint64)), mainWidget, SLOT(UserSendFileSlot(qint64)));
	connect(this, SIGNAL(UserCancelSendFileSignal(qint64)), mainWidget, SLOT(UserCancelSendFileSlot(qint64)));
	connect(this, SIGNAL(UserDlgCloseSignal(unsigned long)), mainWidget, SLOT(UserDlgCloseSlot(unsigned long)));
	connect(this, SIGNAL(UserRefuseRecvFileSignal(qint64)), mainWidget, SLOT(UserRefuseRecvFileSlot(qint64)));
	connect(this, SIGNAL(UserCancelRecvFileSignal(qint64)), mainWidget, SLOT(UserCancelRecvFileSlot(qint64)));
}

UserDlg::UserDlg(unsigned long ID, qint64 fileID, QString fileName, QWidget *parent) :
QWidget(parent),
ui(new Ui::UserDlg)
{
	//m_ID = ID;
	//ui->setupUi(this);
	//setAttribute(Qt::WA_DeleteOnClose);
	//setAcceptDrops(true);
	//ui->fileTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//固定大小
	//ui->fileTableWidget->verticalHeader()->setVisible(false);//不显示横向标题
	//ui->fileTableWidget->horizontalHeader()->setVisible(false);
	//ui->fileTableWidget->setSelectionBehavior(QTableWidget::SelectRows);//一次性先选中一行
	//ui->fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许编辑表格

	//ui->fileTableWidget->insertRow(0);
	//FileTranStatus *statusWidget = new FileTranStatus(1, fileName, this);
	////statusWidget->setStyleSheet("background-color:red");
	//if(statusWidget) {
	//	statusWidget->SetFileID(fileID);	
	//}
	//ui->fileTableWidget->setCellWidget(0, 0, statusWidget);
	//ui->fileTableWidget->resizeColumnsToContents();
	//ui->fileTableWidget->resizeRowsToContents();

}

UserDlg::~UserDlg()
{
	delete ui;

	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter;
	for(iter=sendFileStatusMap.begin(); iter!=sendFileStatusMap.end(); ) {
		if(iter->ip == m_ID) {
			if(iter->tranThread) {
				if(iter->tranThread->isRunning()) {
					emit UserCancelSendFileSignal(iter->fileID);
					((CFileTranThread*)(iter->tranThread))->stop();
					((CFileTranThread*)(iter->tranThread))->wait();
				}
			}
			iter = sendFileStatusMap.erase(iter);
		} else {
			iter++;
		}
	}
	sendFileStatusMapMutex.unlock();

	recvFileStatusMapMutex.lock();
	for(iter=recvFileStatusMap.begin(); iter!=recvFileStatusMap.end();) {
		if(iter->ip == m_ID) {
			if(iter->tranThread) {
				if(iter->tranThread->isRunning()) {
					emit UserCancelRecvFileSignal(iter->fileID);
					((CRecvThread*)(iter->tranThread))->stop();
					((CRecvThread*)(iter->tranThread))->wait();
				}
			}
			iter = recvFileStatusMap.erase(iter);
		} else {
			iter++;
		}
	}
	recvFileStatusMapMutex.unlock();

	emit UserDlgCloseSignal(m_ID);
}

void UserDlg::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

void UserDlg::dropEvent(QDropEvent *event)
{
	QList<QUrl> urlList = event->mimeData()->urls();
	if (urlList.isEmpty())
		return;

	QList<QUrl>::iterator iter;
	for(iter=urlList.begin(); iter!=urlList.end(); iter++) {
		FileStatus fileStaus;
		
		fileStaus.userDlg = this; //用户对话框
		
		fileStaus.ip = m_ID;//ip地址

		qint64 fileID = TimeNo();
		fileStaus.fileID = fileID;//文件ID号
		
		QString filePath = iter->toLocalFile();
		fileStaus.filePath = filePath;//文件路径

		QString fileName;
		QFileInfo file(filePath);
		fileName = file.fileName();
		fileStaus.fileName = fileName;//文件名

		if(file.isDir()) {
			fileStaus.fileType = 1;
		} else {
			fileStaus.fileType = 0;   //文件类型
		}

		fileStaus.sendOrRecv = 0;      //接受还是发送

		ui->fileTableWidget->insertRow(0);
		FileTranStatus *statusWidget = new FileTranStatus(0, fileName, 0, this);
		if(statusWidget) {
			statusWidget->SetFileID(fileID);
			connect(statusWidget, SIGNAL(CancelSignal(qint64)), this, SLOT(CancelSlot(qint64)));
		}
		ui->fileTableWidget->setCellWidget(0, 0, statusWidget);
		ui->fileTableWidget->resizeColumnsToContents();
		ui->fileTableWidget->resizeRowsToContents();

		fileStaus.statusWidget = statusWidget;//状态对话框
		fileStaus.tranThread = NULL;

		sendFileStatusMapMutex.lock();
		sendFileStatusMap.insert(fileID, fileStaus);
		sendFileStatusMapMutex.unlock();

		CFileCount *fileCount = new CFileCount(filePath, fileID);
		m_countFileThreadMap.insert(fileID, fileCount);
        qRegisterMetaType< QList<QString> >("QList<QString>");
		connect(fileCount, SIGNAL(FileSizeSignal(qint64, qint64, QList<QString>, QString)), this, SLOT(FileSizeSlot(qint64, qint64, QList<QString>, QString)));
		fileCount->start();             //取大小的线程
	}
}

unsigned long UserDlg::GetUserID()
{
	return m_ID;
}

qint64 UserDlg::TimeNo(void)
{
	SYSTEMTIME	st;
	qint64	ft;

	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, (FILETIME *)&ft);

	return ft;
}

void UserDlg::SetRecvFile(qint64 fileID, QString fileName, qint64 fileSize, int fileType, QString fileCheck)
{
	int status = 0;
	recvStatusControlMapMutex.lock();
	if(recvStatusControlMap.contains(fileCheck)) {
		status = 1;
	}
	recvStatusControlMapMutex.unlock();

	ui->fileTableWidget->insertRow(0);
	FileTranStatus *statusWidget = NULL;
	if(status == 1) {
		statusWidget = new FileTranStatus(2, fileName, fileSize, this);
	} else {
		statusWidget = new FileTranStatus(1, fileName, fileSize, this);
	}

	if(statusWidget) {
		statusWidget->SetFileID(fileID);
	}
	ui->fileTableWidget->setCellWidget(0, 0, statusWidget);
	ui->fileTableWidget->resizeColumnsToContents();
	ui->fileTableWidget->resizeRowsToContents();
	connect(statusWidget, SIGNAL(RefuseRecvFileSignal()), this, SLOT(RefuseRecvFileSlot()));
	connect(statusWidget, SIGNAL(CancelRecvSignal(qint64)), this, SLOT(CancelRecvSlot(qint64)));

	FileStatus fileStaus;
	fileStaus.fileID = fileID;            //文件ID;
	fileStaus.ip = m_ID;                  //ip
	fileStaus.fileName = fileName;        //文件名
	fileStaus.fileSize = fileSize;        //文件大小
	fileStaus.userDlg = this;             //用户窗口指针
	fileStaus.sendOrRecv = 1;             //接收
	fileStaus.statusWidget = statusWidget;//状态控件指针
	fileStaus.fileType = fileType;        //文件类型
	fileStaus.fileCheck = fileCheck;      //文件识别码
	fileStaus.tranThread = NULL;
	
	recvFileStatusMapMutex.lock();
	recvFileStatusMap.insert(fileID, fileStaus);
	recvFileStatusMapMutex.unlock();
}

void UserDlg::CancelRecvFile(qint64 fileID)
{
	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = recvFileStatusMap.find(fileID);
	if(iter==recvFileStatusMap.end()) {
		recvFileStatusMapMutex.unlock();
		return ;
	}
	if(iter->tranThread) {
		((CRecvThread*)(iter->tranThread))->stop();
		((CRecvThread*)(iter->tranThread))->wait();
	}

	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++) {
		if(iter->statusWidget == (QWidget*)ui->fileTableWidget->cellWidget(i, 0)) {
			ui->fileTableWidget->removeRow(i);
			delete iter->statusWidget;
		}
	}
	recvFileStatusMap.erase(iter);
	recvFileStatusMapMutex.unlock();
}

void UserDlg::CancelSendFile(qint64 fileID)
{
	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = sendFileStatusMap.find(fileID);
	if(iter==sendFileStatusMap.end()) {
		sendFileStatusMapMutex.unlock();
		return ;
	}
	if(iter->tranThread) {
		((CFileTranThread*)(iter->tranThread))->stop();
		((CFileTranThread*)(iter->tranThread))->wait();
	}

	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++) {
		if(iter->statusWidget == (QWidget*)ui->fileTableWidget->cellWidget(i, 0)) {
			ui->fileTableWidget->removeRow(i);
			delete iter->statusWidget;
		}
	}
	sendFileStatusMap.remove(fileID);
	sendFileStatusMapMutex.unlock();
}

void UserDlg::RefuseRecvFileSlot() //接收方拒绝
{
	FileTranStatus *recvFileStatus = (FileTranStatus*)sender();

	qint64 fileID = recvFileStatus->GetFileID();
	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = recvFileStatusMap.find(fileID);
	recvFileStatusMap.erase(iter);
	recvFileStatusMapMutex.unlock();
	
	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++ ) {
		FileTranStatus *fileStatus = (FileTranStatus*)ui->fileTableWidget->cellWidget(i, 0);
		if(fileStatus == recvFileStatus) {
			ui->fileTableWidget->removeRow(i);
			delete recvFileStatus;
			break;
		}
	}
	emit UserRefuseRecvFileSignal(fileID);
}

void UserDlg::RefuseRecvFile(qint64 fileID) //发送方取消
{
	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = sendFileStatusMap.find(fileID);
	sendFileStatusMap.erase(iter);
	sendFileStatusMapMutex.unlock();
	
	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++ ) {
		FileTranStatus *fileStatus = (FileTranStatus*)ui->fileTableWidget->cellWidget(i, 0);
		qint64 ID = fileStatus->GetFileID();
		if(ID == fileID) {
			ui->fileTableWidget->removeRow(i);
			delete fileStatus;
			break;
		}
	}
}

void UserDlg::FileSizeSlot(qint64 fileID, qint64 fileSize, QList<QString> fileList, QString fileCheck)
{
	QMap<qint64, FileStatus>::iterator iter;
	sendFileStatusMapMutex.lock();
	iter = sendFileStatusMap.find(fileID);
	iter->fileSize = fileSize;                    //文件或者文件夹大小
	((FileTranStatus*)(iter->statusWidget))->SetFileSize(fileSize);
	iter->fileList = fileList;
	iter->fileCheck = fileCheck;
	sendFileStatusMapMutex.unlock();
	emit UserSendFileSignal(fileID);
}

void UserDlg::CancelSlot(qint64 fileID)
{
	FileTranStatus *fileTranStatus = (FileTranStatus*)sender();
	QMap<qint64, QThread*>::iterator iter = m_countFileThreadMap.find(fileID);
	CFileCount *fileCountThread = (CFileCount*)(*iter);
	if(fileCountThread->isRunning()) {
		fileCountThread->Stop();
		fileCountThread->wait();
	}
	m_countFileThreadMap.erase(iter);

	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++) {
		FileTranStatus * fileTran = (FileTranStatus *)ui->fileTableWidget->cellWidget(i, 0);
		if(fileTran == fileTranStatus) {
			delete fileTranStatus;
			ui->fileTableWidget->removeRow(i);
		}
	}

	sendFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter1 = sendFileStatusMap.find(fileID);
	if(iter1->tranThread) {
		((CFileTranThread*)(iter1->tranThread))->stop();
		((CFileTranThread*)(iter1->tranThread))->wait();
	}
	sendFileStatusMap.erase(iter1);
	sendFileStatusMapMutex.unlock();
	emit UserCancelSendFileSignal(fileID);
}

void UserDlg::CancelRecvSlot(qint64 fileID) 
{
	emit UserCancelRecvFileSignal(fileID);
	FileTranStatus *fileTranStatus = (FileTranStatus*)sender();

	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = recvFileStatusMap.find(fileID);
	if(iter == recvFileStatusMap.end()) {
		recvFileStatusMapMutex.unlock();
		return;
	}
	if(iter->tranThread) {
		((CRecvThread*)(iter->tranThread))->stop();
		((CRecvThread*)(iter->tranThread))->wait();
	}	
	recvFileStatusMap.erase(iter);
	recvFileStatusMapMutex.unlock();

	int row = ui->fileTableWidget->rowCount();
	for(int i=0; i<row; i++) {
		FileTranStatus * fileTran = (FileTranStatus *)ui->fileTableWidget->cellWidget(i, 0);
		if(fileTran == fileTranStatus) {
			delete fileTranStatus;
			ui->fileTableWidget->removeRow(i);
		}
	}
}
