#include "common.h"
gettrafficmeter_t::gettrafficmeter_t()
{

}

gettrafficmeter_t::~gettrafficmeter_t()
{

}

void gettrafficmeter_t::set(QString &NewTodayConnectionTime, QString &NewTodayUpload, QString &NewTodayDownload, QString &NewYesterdayConnectionTime, QString &NewYesterdayUpload, QString &NewYesterdayDownload, QString &NewWeekConnectionTime, QString &NewWeekUpload, QString &NewWeekDownload, QString &NewMonthConnectionTime, QString &NewMonthUpload, QString &NewMonthDownload, QString &NewLastMonthConnectionTime, QString &NewLastMonthUpload, QString &NewLastMonthDownload)
{
    int pos = 0;
    TodayConnectionTime     = NewTodayConnectionTime;
    TodayUpload             = NewTodayUpload;
    TodayDownload           = NewTodayDownload;
    YesterdayConnectionTime = NewYesterdayConnectionTime;

    pos = NewYesterdayUpload.indexOf("/");
    YesterdayUpload         = NewYesterdayUpload.left(pos);

    pos = NewYesterdayDownload.indexOf("/");
    YesterdayDownload       = NewYesterdayDownload.left(pos);

    WeekConnectionTime      = NewWeekConnectionTime;

    pos = NewWeekUpload.indexOf("/");
    WeekUpload              = NewWeekUpload.left(pos);
    WeekUploadAvg           = NewWeekUpload.right(NewWeekUpload.length()- pos - 1);

    pos = NewWeekDownload.indexOf("/");
    WeekDownload            = NewWeekDownload.left(pos);
    WeekDownloadAvg         = NewWeekDownload.right(NewWeekDownload.length()- pos - 1);

    MonthConnectionTime     = NewMonthConnectionTime;

    pos = NewMonthUpload.indexOf("/");
    MonthUpload             = NewMonthUpload.left(pos);
    MonthUploadAvg          = NewMonthUpload.right(NewMonthUpload.length()- pos - 1);

    pos = NewMonthDownload.indexOf("/");
    MonthDownload           = NewMonthDownload.left(pos);
    MonthDownloadAvg        = NewMonthDownload.right(NewMonthDownload.length()- pos - 1);

    LastMonthConnectionTime = NewLastMonthConnectionTime;

    pos = NewLastMonthUpload.indexOf("/");
    LastMonthUpload         = NewLastMonthUpload.left(pos);
    LastMonthUploadAvg      = NewLastMonthUpload.right(NewLastMonthUpload.length()- pos - 1);

    pos = NewLastMonthDownload.indexOf("/");
    LastMonthDownload       = NewLastMonthDownload.left(pos);
    LastMonthDownloadAvg    = NewLastMonthDownload.right(NewLastMonthDownload.length()- pos - 1);
}

void gettrafficmeter_t::get(QString &NewTodayConnectionTime, QString &NewTodayUpload, \
                            QString &NewTodayDownload, QString &NewYesterdayConnectionTime, \
                            QString &NewYesterdayUpload, QString &NewYesterdayDownload, \
                            QString &NewWeekConnectionTime, QString &NewWeekUpload, \
                            QString &NewWeekDownload, QString &NewMonthConnectionTime, \
                            QString &NewMonthUpload, QString &NewMonthDownload, \
                            QString &NewLastMonthConnectionTime, QString &NewLastMonthUpload, \
                            QString &NewLastMonthDownload)
{
    NewTodayConnectionTime     = TodayConnectionTime;
    NewTodayUpload             = TodayUpload;
    NewTodayDownload           = TodayDownload;
    NewYesterdayConnectionTime = YesterdayConnectionTime;
    NewYesterdayUpload         = YesterdayUpload;
    NewYesterdayDownload       = YesterdayDownload;
    NewWeekConnectionTime      = WeekConnectionTime;
    NewWeekUpload              = WeekUpload;
    NewWeekDownload            = WeekDownload;
    NewMonthConnectionTime     = MonthConnectionTime;
    NewMonthUpload             = MonthUpload;
    NewMonthDownload           = MonthDownload;
    NewLastMonthConnectionTime = LastMonthConnectionTime;
    NewLastMonthUpload         = LastMonthUpload;
    NewLastMonthDownload       = LastMonthDownload;
}

QString gettrafficmeter_t::gettrafficmeterenable()
{
    return TrafficMeterEnable;
}

void gettrafficmeter_t::settrafficmeterenable(QString &NewTrafficMeterEnable)
{
    TrafficMeterEnable = NewTrafficMeterEnable;
}

QString gettrafficmeter_t::gettotaltoday()
{
    QString totaltoday;
    float upload=0;
    float download=0;
    float total=0;

    upload = convert(TodayUpload);
    download = convert(TodayDownload);
    total = upload + download;
    totaltoday = QString("%1").arg(total);

    return totaltoday;
}

QString gettrafficmeter_t::gettotalyesterday()
{
    QString totalyesterday;
    float upload=0;
    float download=0;
    float total=0;

    upload = convert(YesterdayUpload);
    download = convert(YesterdayDownload);
    total = upload + download;
    totalyesterday = QString("%1").arg(total);

    return totalyesterday;
}

QString gettrafficmeter_t::getuploadweekavg()
{
    return WeekUploadAvg;
}

QString gettrafficmeter_t::getdownloadweekavg()
{
    return WeekDownloadAvg;
}

QString gettrafficmeter_t::gettotalweek()
{
    QString totalweek;
    float upload=0;
    float download=0;
    float total=0;

    upload = convert(WeekUpload);
    download = convert(WeekDownload);
    total = upload + download;
    totalweek = QString("%1").arg(total);

    return totalweek;
}

QString gettrafficmeter_t::gettotalweekavg()
{
    QString totalweekavg;
    float uploadavg=0;
    float downloadavg=0;
    float totalavg=0;

    uploadavg = convert(WeekUploadAvg);
    downloadavg = convert(WeekDownloadAvg);
    totalavg = uploadavg + downloadavg;
    totalweekavg = QString("%1").arg(totalavg);

    return totalweekavg;
}

QString gettrafficmeter_t::getuploadmonthavg()
{
    return MonthUploadAvg;
}

QString gettrafficmeter_t::getdownloadmonthavg()
{
    return MonthDownloadAvg;
}

QString gettrafficmeter_t::gettotalmonth()
{
    QString totalmonth;

    float upload=0;
    float download=0;
    float total=0;

    upload = convert(MonthUpload);
    download = convert(MonthDownload);
    total = upload + download;
    totalmonth = QString("%1").arg(total);

    return totalmonth;
}

QString gettrafficmeter_t::gettotalmonthavg()
{
    QString totalmonthavg;
    float uploadavg=0;
    float downloadavg=0;
    float totalavg=0;

    uploadavg = convert(MonthUploadAvg);
    downloadavg = convert(MonthDownloadAvg);
    totalavg = uploadavg + downloadavg;
    totalmonthavg = QString("%1").arg(totalavg);

    return totalmonthavg;
}

QString gettrafficmeter_t::getuploadlastmonthavg()
{
    return LastMonthUploadAvg;
}
QString gettrafficmeter_t::getdownloadlastmonthavg()
{
    return LastMonthDownloadAvg;
}

QString gettrafficmeter_t::gettotallastmonth()
{
    QString totallastmonth;

    float upload=0;
    float download=0;
    float total=0;

    upload = convert(LastMonthUpload);
    download = convert(LastMonthDownload);
    total = upload + download;
    totallastmonth = QString("%1").arg(total);

    return totallastmonth;
}

QString gettrafficmeter_t::gettotallastmonthavg()
{
    QString totallastmonthavg;

    float uploadavg=0;
    float downloadavg=0;
    float totalavg=0;

    uploadavg = convert(LastMonthUploadAvg);
    downloadavg = convert(LastMonthDownloadAvg);
    totalavg = uploadavg + downloadavg;
    totallastmonthavg = QString("%1").arg(totalavg);

    return totallastmonthavg;
}

float gettrafficmeter_t::convert(QString &numstr)
{
   return numstr.remove(",").toFloat();
}

void settrafficmeter_t::set(QString &NewTrafficMeterEnable, QString &NewControlOption, QString &NewMonthlyLimit, QString &RestartHour, QString &RestartMinute, QString &RestartDay)
{
    this->NewTrafficMeterEnable = NewTrafficMeterEnable;
    this->NewControlOption = NewControlOption;
    this->NewMonthlyLimit = NewMonthlyLimit;
    this->RestartHour = RestartHour;
    this->RestartMinute = RestartMinute;
    this->RestartDay = RestartDay;
}

void settrafficmeter_t::get(QString &NewTrafficMeterEnable, QString &NewControlOption, QString &NewMonthlyLimit, QString &RestartHour, QString &RestartMinute, QString &RestartDay)
{
    NewTrafficMeterEnable = this->NewTrafficMeterEnable;
    NewControlOption = this->NewControlOption;
    NewMonthlyLimit = this->NewMonthlyLimit;
    RestartHour = this->RestartHour;
    RestartMinute = this->RestartMinute;
    RestartDay = this->RestartDay;
}
