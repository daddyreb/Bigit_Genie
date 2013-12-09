#ifndef QGENIEEXPORT2DISKFILETHREAD_H
#define QGENIEEXPORT2DISKFILETHREAD_H

#include <QThread>
#include <QString>


class QGenieExport2DiskFileThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieExport2DiskFileThread(QObject *parent = 0);
    ~QGenieExport2DiskFileThread();
protected:
#ifdef PLATFORM_WIN32
    QString                         m_strnicguid;
#endif
    QString                         m_strssid;
    QString                         m_strprofilename_or_pwd;
    QString                         m_auth;
    QString                         m_cipher;
    QString                         m_strflashdisk;
public:
    bool begin_export(const QString &strnicguid,
                      const QString &strssid,
                      const QString &strprofilename_or_pwd,
                      const QString &auth,
                      const QString &cipher,
                      const QString &strflashdisk );
protected:
    void run();
signals:
    void export_completed(bool bok);
public slots:

private:
    //export mac os x app bundle
    bool copyFile(const QString &filePath,const QString &destDir);
    bool copyMacOSAppBundle(const QString &destDir);
};

#endif // QGENIEEXPORT2DISKFILETHREAD_H
