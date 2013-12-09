#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QStackedWidget>
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/genie_interface/ISoapManager.h"
#include "inc/genie_interface/ISettings.h"
#include "QProgressIndicator.h"
#include "soapproxy.h"

#include "trafficsetting.h"
#include "trafficstatistics.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent);
    ~MainWindow();

private:
    void retranslateUi();

private:
    ISettings * mSettings;

    QProgressIndicator * progressindicator;
    QProgressIndicator * progressindicator_2;

    int m_NewControlOption;
    int m_NewMonthlyLimit;
    int m_RestartHour;
    int m_RestartMinute;
    int m_RestartDay;
    int m_EnableTrafficMeter;

    QIntValidator *hourValidator;
    QIntValidator *minuteValidator;
    QIntValidator *monthLimitValidator;
    QLabel *errorLabel;

//    QStackedLayout * statckedlayout;
    QStackedWidget *stackedWidget;

public:
    static IManagerHandler *sHandler;
    ILanguageManager* lanmanager;
    ISoapManager *soapmanager;
    NetgearSoapSchedule *soapapi;
    QUuid uuid;
    void JudgeNetgear();
    void GetTrafficStatistics();
    void setwaitstr(QString&);
    void setrefreshstr(QString&);
    void applyend();
    void refreshend();
    void initdata();

private slots:
    void JudgeEnable();
    void GetMessage();
    void GetResponseEnable(QtSoapMessage&,QUuid&,QString&);
    void GetResponseStatistics(QtSoapMessage&,QUuid&,QString&);
    void Refresh();
    void Apply();
    void Cancel();
    void startAnimation();
    void stopAnimation();
    void changesecondpage();

public slots:
    void changefirstpage();
    void changefirstpage2();

private:
    TrafficStatistics * trafficstatistics;
    TrafficSetting * trafficsetting;
    SoapProxy *soapproxy;
    IUiManager * uimanager;

protected:
    virtual void changeEvent(QEvent* event);

signals:
    void setwaitstrsignal(QString &);
    void setrefreshsignal(QString &);
    void applyendsignal();
};

#endif // MAINWINDOW_H
