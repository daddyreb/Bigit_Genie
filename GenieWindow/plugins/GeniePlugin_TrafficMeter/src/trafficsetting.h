#ifndef TRAFFICSETTING_H
#define TRAFFICSETTING_H

#include <QWidget>
#include <QIntValidator>
#include "common.h"

#include "ILogManager.h"

namespace Ui {
    class TrafficSetting;
}

class TrafficSetting : public QWidget {
    Q_OBJECT
public:
    TrafficSetting(QWidget *parent = 0);
    ~TrafficSetting();
    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TrafficSetting *ui;
    ILanguageManager* lanmanager;
    ILogManager *mLogmanager;
    QUuid uuid;
    QProgressIndicator * progressindicator;

    QIntValidator *hourValidator;
    QIntValidator *minuteValidator;
    QIntValidator *monthLimitValidator;

    QString tmp, NewControlOption,NewMonthlyLimit,RestartHour,RestartMinute,RestartDay;

    void retranslateUi();
    void showError(int index=CLEAR_MESSAGE);
    void updateview();
    QString boldword(QString str);

    ISettings * mSettings;
    IUiManager * uimanager;

    bool flag;

signals:
//    void applybtclicked();
    void returnbtclicked();
    void changepage();
    void settrafficmeter(settrafficmeter_t &);

public slots:
    void apply();
    void startAnimation();
    void stopAnimation();
    void response(bool,int);
    void initdata();
    void checkboxchange();
    void modifyslot();
    void setwaitstr(QString &);
    void setrefreshstr(QString &);
    void response(bool,int,settrafficmeter_t&);

private slots:
    void startextraturn();
    void startextrarefresh();
    void stopextrarefresh();
};

#endif // TRAFFICSETTING_H
