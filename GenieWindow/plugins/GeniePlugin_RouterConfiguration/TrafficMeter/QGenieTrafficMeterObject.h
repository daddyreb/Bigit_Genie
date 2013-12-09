#ifndef QGENIETRAFFICMETEROBJECT_H
#define QGENIETRAFFICMETEROBJECT_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
#include "chart.h"
class QPiObject;

class QGenieTrafficMeterObject : public QObject,public Singleton<QGenieTrafficMeterObject>
{
    Q_OBJECT
public:
    explicit QGenieTrafficMeterObject(QObject *parent = 0);
    QWidget *getUi();
    void translateUi();
//    void whenLogin();
//    void tabClicked();

protected:
    QWidget *mW;
    QPiObject *mPi;
//    bool mbRefreshAfterLogin;

    int mSessionTMOption;
    int mSessionTMStatistics;
    bool mSavedEnableState;

    QLabel *mErrorLabel;
    int mErrorLan;
    void setError(int lanidx);

    QTableWidget *table_statistics;
    QRadioButton *rdt_enable;
    QRadioButton *rdt_disable;
    QPushButton *but_modify;
    QPushButton *but_apply;
    void sessionOptionReturned(const QStringList &lst);
    void sessionStatisticsReturned(const QStringList &lst);
    void setTMOptions();

    QString mSavedMonthLimit;
    QString mSavedResetHour;
    QString mSavedResetMinute;
    int mSavedLimitType;
    int mSavedResetDay;
    int mSavedMorning;

	///////2012-3-7 chensi////////////
	void deployIndicators(QWidget *w);
private:
        Chart * totalchart, *avgchart;
        QComboBox *combobox;
        QStackedWidget * stackedwidget;
        void refreshData(bool remote);

signals:
    ;

private slots:
    ;

public slots:
    void slot_RefreshData();
    void slot_buttonClicked();
    void slot_radioButclicked();
    void slot_uiModified();
    void slot_DataRreturned(int session, const QStringList &lst);

};

#endif // QGENIETRAFFICMETEROBJECT_H
