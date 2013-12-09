#ifndef TRAFFICSTATISTICS_H
#define TRAFFICSTATISTICS_H

#include <QWidget>
#include "messagebox.h"
#include "QButtonGroup"
#include "common.h"
#include "QGenieMsgDialog.h"

#include "ILogManager.h"
#include "IClickManager.h"

namespace Ui {
    class TrafficStatistics;
}

class TrafficStatistics : public QWidget {
    Q_OBJECT
public:
    TrafficStatistics(QWidget *parent = 0);
    ~TrafficStatistics();
    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TrafficStatistics *ui;
    ILanguageManager* lanmanager;
    ILogManager *mLogmanager;
    IUiManager * uimanager;
    QUuid uuid;
    QProgressIndicator * progressindicator;
    MessageBox *messagebox;
    QButtonGroup * buttongroup;
    QGenieMsgDialog * mesdialog;
    QRadioButton * currentselectedbt;
    QString intervaltime;
    void retranslateUi();
    void showError(int index=CLEAR_MESSAGE);
    void updateview();
    bool flag;

signals:
    void refreshbtclicked();
    void modifybtclicked();
    void logoutbtclicked();
    void changepage();
    void settrafficmeterenable(settrafficmeter_t &);
    void gettrafficmeteroptions();

public slots:
    void startAnimation();
    void stopAnimation();
    void response(bool,int);
    void refreshdata();
    void response(bool,int,gettrafficmeter_t&);
    void initdata();
    void setwaitstr(QString &);
    void setrefreshstr(QString &);
    void startextrarefresh();
    void stopextrarefresh();

private slots:
    void radiobuttonchange(QRadioButton *bt);
    void checkedenableradio(bool );
    void checkeddisableradio(bool );
    void startextrapply();
};
extern QString trafficmeterenable;
#endif // TRAFFICSTATISTICS_H
