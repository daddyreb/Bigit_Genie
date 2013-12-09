#ifndef DOWNLOADWIZARD_H
#define DOWNLOADWIZARD_H

#include <QWidget>
#include "wizardpage.h"

class QProgressBar;

class DownloadFirmware;

class DownloadWizard : public WizardPage
{
    Q_OBJECT
public:
    DownloadWizard(QWidget *parent=0);
    ~DownloadWizard();

    //void Download(QStringList &Info, QString**mInfo);
    void Download(QStringList &Info, QString**mInfo, QList<int> **err);
    //void SetInfo(const QString &Info);
    void SetInfo(const QList<int> &Info);
    void SetNextButton(bool stat);
    void SetBackButton(bool stat);
    void CancelDownload();
    void DestructorDownload();

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);


private slots:
    void GetFirmware(bool err);
    void ViewLeaveTime(int msec);

signals:
    void DownloadFinish(bool err);

private:
    QWidget *m_Widget;
    QLabel *ViewTime;
    QLabel *TimeLabel;
    QLabel *ViewInfo;
    QProgressBar *DownloadProgress;
    ILanguageManager *lanmanager;
    QList<int> vInfo;

    QString *outInfo;
    QList<int> *intErr;
    QString *tempErr;

    DownloadFirmware *mDownload;

};


#endif // DOWNLOADWIZARD_H
