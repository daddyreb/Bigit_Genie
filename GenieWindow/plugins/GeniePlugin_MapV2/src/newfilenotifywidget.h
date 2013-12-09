//add by caiyujun
#ifndef NEWFILENOTIFYWIDGET_H
#define NEWFILENOTIFYWIDGET_H

#include <QWidget>
#include <QString>
#include <QPoint>
#include <QProgressBar>

#include "udt.h"
//#include "api.h"



class QPushButton;
class QLabel;
class QTextEdit;
class QPropertyAnimation;

class NewFileNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NewFileNotifyWidget(int type, QString strDeviceName, int fhandle, int iFolder, uint64_t size, QWidget *parent = 0);
    explicit NewFileNotifyWidget(int iFolder, QWidget *parent = 0);
    ~NewFileNotifyWidget();
private:
    QPushButton         *m_closeBtn;
    QPushButton         *m_saveBtn;
    QPushButton         *m_timertocloseBtn;
    QPushButton         *m_rejectBtn;
	QPushButton			*m_okBtn;
	QPushButton			*m_openBtn;
	QPushButton			*m_openFolderBtn;
    QPushButton         *m_cancelBtn; ///kai.yan add
    QLabel              *m_titleLabel;
    QLabel              *m_titleContentLabel;
    QTextEdit           *m_bodyEdit;
    QProgressBar        *m_progressBar;

    bool                m_canMove;
    QPoint              m_pos;
    QPropertyAnimation *m_notifyAnimation;
    int                 m_timeoutCounter;
	int					m_currentPercent;
    QString             m_strFileName;
    QString             m_strFile;
    QString             m_strDeviceName;
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

    UDTSOCKET m_fhandle;
    int m_iFolder;
    qint64 m_size;
    QString m_strFolder;
	int m_iType;
	QString m_strLastFolder;
	QString m_strCurrentFile;
	QStringList m_strFileList;
	bool m_bSendOver;
   // bool m_sender;


signals:
    void translateText(int idx ,QString *text);
    void completed(int countRemain);
    void timeout();
    void saveFile(QString strFilePath, int m_iFolder, int fhandle);
    void rejectSaveFile(int iFolder, int fhandle);
    void cancelReceive(); ///kai.yan add
	void saveResult(int sock, QString strFilePath);

	void stopTransfer(int sock);
	void folderChange(QString strFolder);
private slots:
    void rejectBtnClicked();
	void openBtnClicked();
	void openFolderBtnClicked();
    void okBtnClicked();
	void saveBtnClicked();
    void closeBtnClicked();
    void cancelBtnClicked();


public:
    void notifyNewDevice(const QString &title,int defaultCount = 0);
	void closeWidget(int status);
	void transferPercent(double percent,double speed,QString filename);
	void finishReuslt(int nReturnCode);

	void setSock(int sock);
	QString getActualName(QString strFile);
	void setLastFolder(QString strLastFolder);

};

#endif // NEWFILENOTIFYWIDGET_H
