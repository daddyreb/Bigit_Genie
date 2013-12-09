/*-------------------------------------
  @author jianghua
  *this class is create system tray ico
  *class Function
   main window hide , this application show tray icon
 -----------------------------------------*/

#ifndef QGENIETRAYMANAGER_H
#define QGENIETRAYMANAGER_H
#include <QtCore>
#include <QtGui>
#include <QDomDocument>
class IGenieTrayIco;


class QGenieTrayManager :public QObject
{
    Q_OBJECT
public:
    QGenieTrayManager(int type=1);
    ~QGenieTrayManager();

//    void createTrayMenu(QDomDocument &xmlDoc);
//    void createTrayMenu(const QString &uiFName);
//    void parseUiCommand(int cmd,const QVariant &paras);
//    void setMessage(const QString &strText);  // set tray icon message
//    void setToolTipText(const QString &tooltip);               // set Tooltip Text
//    void setIcon(const QString &ico);
//    void setLayoutLeftToRight(bool ltor);

//    void changeText();



    void uiCommand(int cmd,const QString &para=QString());
//    void initUi(bool b1,bool b2);
//    void uiAction(int cmd,const QString &ui_item,const QVariant &para=QVariant());
signals:
    void actionTrigged(const QString &);
    void canInitNow();
private :
    IGenieTrayIco *mTrayIco;
    QWidget *mMainWnd;
//    void trayIconClicked();


//    void _saveAutoStartState(bool b);
    void _startUpdate();

    friend class QGenieTrayIco_In;
    friend class QGenieTrayIco_Out;

};

#endif // QGENIETRAYMANAGER_H
