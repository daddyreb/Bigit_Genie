#include "filetranstatus.h"
#include "ui_filetranstatus.h"
#include <QFile>
#include <QFileDialog>
#include "IPFileMsg.h"
#include "recvthread.h"
#include <QMutex>

extern QMap<qint64, FileStatus> recvFileStatusMap;
extern QMutex recvFileStatusMapMutex;

extern QMap<qint64, FileStatus> sendFileStatusMap;
extern QMutex sendFileStatusMapMutex;

extern QMap<QString, RecvStatus> recvStatusControlMap;
extern QMutex recvStatusControlMapMutex;

FileTranStatus::FileTranStatus(unsigned long cmd, QString fileName, qint64 fileSize, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileTranStatus)
{
	m_cmd = cmd;
	m_fileName = fileName;
	
	m_fileID = 0;
	m_status = 0;

    ui->setupUi(this);
	ui->fileNameLabel->setText(fileName);

	ui->fileSizeLabel->setText(QString("%1").arg(fileSize));
	ui->fileNameLabel->setText(fileName);
	//ui->progressBar->alignment(Qt::AlignCenter);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(100);
	ui->progressBar->setTextVisible(false); 
	ui->progressBar->setValue(0);
	if(0 == cmd) {//����
		ui->sendRecvStatusLabel->setText(QString::fromLocal8Bit("�����ļ�"));
		ui->recvButton->hide();
		ui->saveButton->hide();
		ui->cancelRecvButton->hide();
		ui->continueButton->hide();
		ui->refuseButton->hide();
	}
	if(1 == cmd) {//�����ļ�
		ui->sendRecvStatusLabel->setText(QString::fromLocal8Bit("�����ļ�"));
		ui->cancelButton->hide();
		ui->cancelRecvButton->hide();
		ui->continueButton->hide();
	}
	if(2 == cmd) {//�����ļ�
		ui->cancelRecvButton->hide();
		ui->sendRecvStatusLabel->setText(QString::fromLocal8Bit("�����ļ�"));
		ui->cancelButton->hide();
		ui->recvButton->hide();
	}
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(CancelSlot())); //ȡ������
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(ChooseFilePath())); //����
	connect(ui->recvButton, SIGNAL(clicked()), this, SLOT(SaveFilePath())); //ֱ�ӽ���
	connect(ui->refuseButton, SIGNAL(clicked()), this, SLOT(RefuseRecvFile()));//�ܾ�����
	connect(ui->continueButton, SIGNAL(clicked()), this, SLOT(ContinueRecvFile()));//�����ļ�
	connect(ui->cancelRecvButton, SIGNAL(clicked()), this, SLOT(CancelRecvSlot()));//ȡ������
}

FileTranStatus::~FileTranStatus()
{
    delete ui;
}

void FileTranStatus::ContinueRecvFile()
{
	m_cmd = 2;
	QString dir;
	QString checkFile;
	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = recvFileStatusMap.find(m_fileID);
	checkFile = iter->fileCheck;
	recvFileStatusMapMutex.unlock();
	
	recvStatusControlMapMutex.lock();
	QMap<QString, RecvStatus>::iterator iter1 = recvStatusControlMap.find(checkFile);
	dir = iter1->filePath;
	recvStatusControlMapMutex.unlock();
	ui->continueButton->hide();
	StartRecvFile(dir);
}

void FileTranStatus::ChooseFilePath()
{
	QString dir = QFileDialog::getSaveFileName(this, tr("Save File"),
		QString(QString::fromLocal8Bit("./../Download/%1")).arg(m_fileName),
		QString::fromLocal8Bit("�����ļ� (*.*)"));
	if(dir.isEmpty()){
		return;
	}

	StartRecvFile(dir);
}

void FileTranStatus::SaveFilePath()
{
	QString dir = QString::fromLocal8Bit("./../Download/%1").arg(m_fileName);
	StartRecvFile(dir);
}

void FileTranStatus::StartRecvFile(QString fileSavePath)
{
	ui->recvButton->hide();
	ui->saveButton->hide();
	ui->refuseButton->hide();
	//ui->cancelButton->show();
	ui->cancelRecvButton->show();
	m_status = 2;

	CRecvThread *recvThread = new CRecvThread(fileSavePath, m_fileID, m_cmd);
	connect(recvThread, SIGNAL(RecvThreadErrorSignal(int)), this, SLOT(RecvThreadErrorSlot(int)));
	recvFileStatusMapMutex.lock();
	QMap<qint64, FileStatus>::iterator iter = recvFileStatusMap.find(m_fileID);
	QFileInfo fileInfo(fileSavePath);
	QString fileName = iter->fileName = fileInfo.fileName();//�����ļ���
	QString filePath = iter->filePath = fileInfo.filePath();//�����ļ�·��
	iter->tranThread = recvThread;
	recvFileStatusMapMutex.unlock();
	connect(recvThread, SIGNAL(RecvProgressSignal(qint64, qint64)), this, SLOT(PogressSlot(qint64, qint64)));
	connect(recvThread, SIGNAL(RecvCompleteSignal()), this, SLOT(CompleteSlot()));
	connect(recvThread, SIGNAL(RecvOpenFileErrorSignal()), this , SLOT(RecvOpenFileErrorSlot()));
	connect(recvThread, SIGNAL(RecvThreadErrorSignal(int)), this, SLOT(RecvThreadErrorSlot()));
	recvThread->start();
}

void FileTranStatus::RecvThreadErrorSlot(int err)
{
	ui->fileSizeLabel->setText(QString::fromLocal8Bit("������������жϣ���������"));
	ui->cancelButton->hide();
	ui->cancelRecvButton->hide();
}

void FileTranStatus::RefuseRecvFile()
{
	emit RefuseRecvFileSignal();
}

void FileTranStatus::SetFileSize(qint64 fileSize)
{
	ui->fileSizeLabel->setText(QString("%1").arg(fileSize));
}

void FileTranStatus::CancelSlot()
{
	emit CancelSignal(m_fileID);
}

void FileTranStatus::PogressSlot(qint64 sum, qint64 complete)
{
	qint64 a = (complete*100/sum);
	if(a ==1) {
		a = 1;
	}
	if(a > 100)
	{
		a = a;
	}
	char sendPercent[100];
	memset(sendPercent, 0, 100);
	sprintf(sendPercent, "%I64d%%", a);
	ui->fileSizeLabel->setText(sendPercent);
	ui->progressBar->setValue(a);
}

void FileTranStatus::CompleteSlot()
{
	ui->fileSizeLabel->setText(QString::fromLocal8Bit("�ļ��������"));
	ui->cancelButton->hide();
	ui->cancelRecvButton->hide();
}

void FileTranStatus::CancelRecvSlot()
{
	emit CancelRecvSignal(m_fileID);
}

void FileTranStatus::RecvOpenFileErrorSlot()
{
	ui->fileSizeLabel->setText(QString::fromLocal8Bit("�ļ������ļ���д����"));
	ui->cancelButton->hide();
	ui->cancelRecvButton->hide();
}