#ifndef QGENIEMAPDBGWOBJECT_H
#define QGENIEMAPDBGWOBJECT_H
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
class QGenieMapDbgWObject:public QObject
{
    Q_OBJECT
public:
    QGenieMapDbgWObject(QObject *parent=NULL);
    QWidget *getW();
protected slots:
    void slot_RdtClicked(bool);
protected:
    QWidget *mW;
};

#endif // QGENIEMAPDBGWOBJECT_H
