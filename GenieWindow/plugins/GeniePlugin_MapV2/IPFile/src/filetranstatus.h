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
    explicit FileTranStatus(unsigned long cmd, QString fileName, qint64 fileSize, QWidget *parent = 0); //cmd = 0,1,2 0���ͣ� 1���գ� 2����
    ~FileTranStatus();

private:
    Ui::FileTranStatus *ui;
	unsigned long m_cmd;
	QString m_fileName;
	qint64 m_fileID;
	int m_status;   //״̬ ����ȡ����ť��״̬ת�� 0 ���ڷ��� �Է���û���գ� 1 ���ڷ��ͣ��Է��Ѿ����ܣ� 2�Ѿ���ʼ����
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
