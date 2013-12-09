#ifndef GETSYSTEMDETAILS_H
#define GETSYSTEMDETAILS_H

#include <QObject>

#include <QStringList>
#include <QMap>
#include <QNetworkInterface>
#include <QLibrary>


class GetSystemDetails : public QObject
{
    Q_OBJECT
public:
    GetSystemDetails();
    ~GetSystemDetails();

	void SystemDetailsNew(QMap<QString, QMap<QString, QStringList> > &OutData);

signals:
    void GetDetailsFinish();

private:

	void GetNetworkInfo(QMap<QString, QMap<QString, QStringList> > &OutData);
	void GetOperationInfo_sys(QMap<QString, QMap<QString, QStringList> >&OutData);

    std::list<std::string> StringSplit(std::string mString, char &mSplit);


    QString mdllPath;
    QLibrary *mDllLibrary;
    QLibrary *sysDllLibrary;
    QLibrary *netDllLibrary;

    QMap<QString, QStringList> cDetailsMap;

    QStringList mDetails;

    QNetworkInterface otherDetails;

};

#endif // GETSYSTEMDETAILS_H
