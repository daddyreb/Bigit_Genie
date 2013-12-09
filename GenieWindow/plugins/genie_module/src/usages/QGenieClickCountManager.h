/*-------------------------------------
  @author jianghua
  *this class is create system tray ico
  *class Function
   main window hide , this application show tray icon
 -----------------------------------------*/

#ifndef QGENIECLICKCOUNTMANAGER_H
#define QGENIECLICKCOUNTMANAGER_H

#include <QtCore>
#include <QObject>
#include <QSystemTrayIcon>
#include <QAction>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QList>
#include "Singleton.h"

class QGenieClickCountManager :public QObject, public Singleton<QGenieClickCountManager>
{
    Q_OBJECT
public:
    QGenieClickCountManager();


};

#endif // QGenieClickCountManager_H
