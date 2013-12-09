#ifndef GETTHREAD_H
#define GETTHREAD_H

#include <QThread>
#include <QMap>
#include <QStringList>

#include "getsystemdetails.h"

class GetThread : public QThread
{
    Q_OBJECT
public:
    GetThread(QObject *parent=0);
    ~GetThread();

    void StartGetDetails(QMap<QString, QMap<QString, QStringList> > * *OutData);

protected:
     void run();


signals:
    void ThreadFinish();

public slots:
    void ThreadOver();

private:
  //  GetSystemDetails getDetails;
    QMap<QString, QMap<QString, QStringList> > *paraMap;

};




#endif // GETTHREAD_H
