#ifndef QGENIETITLEUPDATE_H
#define QGENIETITLEUPDATE_H

#include <QObject>
#include <QTimer>

class QGenieTitleUpdate : public QObject
{
    Q_OBJECT
public:
    QGenieTitleUpdate(QObject *parent = NULL);
    ~QGenieTitleUpdate(void);

    void command(const QString &cmd);
    void butClicked();
    QString getTitleText();
private:

protected:
    QTimer mTimerSoftwareOrFirmware;
    quint32 mUpdatFuncFlag;
    quint32 mCurShowingFlag;
public slots:
    void slot_SoftwareOrFirmwareTimeout();
signals:
    void updateIcon();

};


#endif
