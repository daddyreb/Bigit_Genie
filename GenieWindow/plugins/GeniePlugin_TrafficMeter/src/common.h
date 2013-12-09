#ifndef COMMON_H
#define COMMON_H

#include <QUuid>
#include <QtDebug>
#include "inc/genie_interface/IManagerHandler.h"
#include "QProgressIndicator.h"
#include "inc/multilanguage_def/LanguageTrafficMeter.h"
#include "inc/plugin_ui_def/plugin_ui_def.h"
#include "inc/genie_interface/IUiManager.h"
#include "ILanguageManager.h"
#include "inc/genie_interface/ISoapManager.h"

#define CLEAR_MESSAGE -1
#define SHOW_LASTEST_MESSAGE -2

class gettrafficmeter_t
{
public:
    gettrafficmeter_t();
    ~gettrafficmeter_t();
    void set(QString &NewTodayConnectionTime, \
             QString &NewTodayUpload, \
             QString &NewTodayDownload, \
             QString &NewYesterdayConnectionTime, \
             QString &NewYesterdayUpload, \
             QString &NewYesterdayDownload, \
             QString &NewWeekConnectionTime, \
             QString &NewWeekUpload, \
             QString &NewWeekDownload, \
             QString &NewMonthConnectionTime, \
             QString &NewMonthUpload, \
             QString &NewMonthDownload, \
             QString &NewLastMonthConnectionTime, \
             QString &NewLastMonthUpload, \
             QString &NewLastMonthDownload);
    void get(QString &NewTodayConnectionTime, \
             QString &NewTodayUpload, \
             QString &NewTodayDownload, \
             QString &NewYesterdayConnectionTime, \
             QString &NewYesterdayUpload, \
             QString &NewYesterdayDownload, \
             QString &NewWeekConnectionTime, \
             QString &NewWeekUpload, \
             QString &NewWeekDownload, \
             QString &NewMonthConnectionTime, \
             QString &NewMonthUpload, \
             QString &NewMonthDownload, \
             QString &NewLastMonthConnectionTime, \
             QString &NewLastMonthUpload, \
             QString &NewLastMonthDownload);
    QString gettotaltoday();
    QString gettotalyesterday();
    QString getuploadweekavg();
    QString getdownloadweekavg();
    QString gettotalweekavg();
    QString gettotalweek();
    QString getuploadmonthavg();
    QString getdownloadmonthavg();
    QString gettotalmonthavg();
    QString gettotalmonth();
    QString getuploadlastmonthavg();
    QString getdownloadlastmonthavg();
    QString gettotallastmonthavg();
    QString gettotallastmonth();
    QString gettrafficmeterenable();
    void settrafficmeterenable(QString &NewTrafficMeterEnable);

private:
    QString TodayConnectionTime;
    QString TodayUpload;
    QString TodayDownload;
    QString YesterdayConnectionTime;
    QString YesterdayUpload;
    QString YesterdayDownload;
    QString WeekConnectionTime;
    QString WeekUpload;
    QString WeekUploadAvg;
    QString WeekUploadUnit;
    QString WeekDownload;
    QString WeekDownloadAvg;
    QString WeekDownloadUnit;
    QString MonthConnectionTime;
    QString MonthUpload;
    QString MonthUploadAvg;
    QString MonthUploadUnit;
    QString MonthDownload;
    QString MonthDownloadAvg;
    QString MonthDownloadUnit;
    QString LastMonthConnectionTime;
    QString LastMonthUpload;
    QString LastMonthUploadAvg;
    QString LastMonthUploadUnit;
    QString LastMonthDownload;
    QString LastMonthDownloadAvg;
    QString LastMonthDownloadUnit;
    QString TrafficMeterEnable;

    float convert(QString &);
};

class settrafficmeter_t
{
public:
    settrafficmeter_t(){};
    ~settrafficmeter_t(){};
    void set(QString &NewTrafficMeterEnable, \
             QString &NewControlOption, \
             QString &NewMonthlyLimit, \
             QString &RestartHour, \
             QString &RestartMinute, \
             QString &RestartDay);
    void get(QString &NewTrafficMeterEnable, \
             QString &NewControlOption, \
             QString &NewMonthlyLimit, \
             QString &RestartHour, \
             QString &RestartMinute, \
             QString &RestartDay);

    QString NewTrafficMeterEnable;
    QString NewControlOption;
    QString NewMonthlyLimit;
    QString RestartHour;
    QString RestartMinute;
    QString RestartDay;
};

#define PLUGIN_UUID "{35b9be0c-b073-4cf8-bc12-d11923dc3d49}"
#endif // COMMON_H
