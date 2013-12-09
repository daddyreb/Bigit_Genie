#ifndef QGENIERCUIMANAGER_H
#define QGENIERCUIMANAGER_H

#include <QObject>

class QGenieRCMainWnd;
class QGenieRCUiManager : public QObject
{
    Q_OBJECT
public:
    explicit QGenieRCUiManager(QObject *parent = 0);
    void createWidgets();

signals:

public slots:
protected:
//    QGenieRCMainWnd *mMainWnd;
};

#endif // QGENIERCUIMANAGER_H
