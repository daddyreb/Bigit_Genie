#ifndef FILETRANSTATUS_H
#define FILETRANSTATUS_H

#include <QWidget>

namespace Ui {
    class FileTranStatus;
}

class FileTranStatus : public QWidget
{
    Q_OBJECT

public:
    explicit FileTranStatus(unsigned long cmd, QString fileName, qint64 fileSize, QWidget *parent = 0); //cmd = 0,1,2 0发送， 1接收， 2续传
    ~FileTranStatus();

private:
    Ui::FileTranStatus *ui;
	unsigned long m_cmd;
	QString m_fileName;
	qint64 m_fileID;
	int m_status;   //状态 用于取消按钮的状态转换 0 正在发送 对方还没接收， 1 正在发送，对方已经接受， 2已经开始接收
public:
	qint64 GetFileID() {return m_fileID;}
	void SetFileID(qint64 fileID) { m_fileID = fileID;}
	void SetFileSize(qint64 fileSize);
	private slots:
		void ChooseFilePath();
		void RefuseRecvFile();
		void ContinueRecvFile();
		void CancelSlot();
		void SaveFilePath();
		void CancelRecvSlot();
		void RecvOpenFileErrorSlot();
	public slots:
		void RecvThreadErrorSlot(int err);
		void PogressSlot(qint64 sum, qint64 complete);
		void CompleteSlot();
signals:
		void RefuseRecvFileSignal();
		void CancelSignal(qint64);
		void CancelRecvSignal(qint64);
private:
        void StartRecvFile(QString fileSavePath);
};

#endif // FILETRANSTATUS_H
