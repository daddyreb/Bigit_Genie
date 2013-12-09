#ifndef AIRPRINTBACKGROUD_H
#define AIRPRINTBACKGROUD_H
#include <QMap>
#include <QLibrary>
#include <QPrinter>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QThread>

typedef struct
{
    char  name[1024];
}PrinterAttr;
typedef struct
{
    PrinterAttr attr[10];
    int count;
}PrinterList;
typedef struct PRINTERLIST_s{
        char printername[10][1024];
}PRINTERLIST_t;

const short ITUNESINSTALLED = 1,
            ADOBEREADERINSTALLED = 2,
            ALLINSTALLED = 3;

typedef void(* pGetFileName)(const char * filename,const char *printername, int type);
typedef int (* pEnumPrintersFunc)(PrinterList * printerlist);
typedef int (* pRegisterPrinter)(const char * printername);
typedef int (* pUnregisterPrinter)(const char * printername);
typedef int (* pInit)(pGetFileName getfilename);
typedef void (* pGetAdobeType)(const int type);
typedef int (* pDestroy)();

class PrintingThread : public QThread
{

public:
    PrintingThread(QObject *parent = 0);
    ~PrintingThread();

    void setPrintingInfo(QString printername, QString filename,int type);

private:
    QQueue<QString> filenamequeue;
    QQueue<QString> printernamequeue;
    QQueue<int> typequeue;
    bool exitflag;

    QMutex mutex;
    QWaitCondition waitcondition;

protected:
    void run();
};

class AirprintBackgroud : public QThread
{
    Q_OBJECT

public:
    AirprintBackgroud(QObject *parent = 0);
    ~AirprintBackgroud();
    static PrintingThread * printingthread;
    static void getFileName(const char *, const char *, int);
    void setRegisterPrinterName(QString printername);
    void setUnregisterPrinterName(QString printername);
    void setRegisterPrinterNames(QList<QString> printernames);
    void setUnregisterPrinterNames(QList<QString> printernames);
    void setRefreshPrinterList();

public slots:
    void setCheckInstallationState();

protected:
    void run();

private:

    QMap<QString,bool> selectedprintermap;

    QLibrary * airprinterdll;
    PrinterList printerlist;
    pEnumPrintersFunc enumPrinters;
    pRegisterPrinter registerPrinter;
    pUnregisterPrinter unregisterPrinter;
	pGetAdobeType getAdobeType;
    pInit initdll;
    pDestroy destroydll;
    short status;

    QMutex mutex;
    QWaitCondition waitcondition;
    bool exitflag,
         checkinstallationstatusflag,
         refreshprinterlistflag;

    QQueue<QString> registerprinternamequeue;
    QQueue<QString> unregisterprinternamequeue;

    void enumPrinterNameList();
    bool loadLibrary(QString &);
    void registerPrinterName(QString printername);
    void unregisterPrintername(QString printername);
    void checkInstallationStatus();
    void registerInitPrinterNameList();

signals:
    void sendSaveConfig(QMap<QString,bool>);
    void sendStatus(short status);
    void sendPrinterNameList(QMap<QString,bool>);
    void sendMessage(QString,bool);
    void sendPrinterStatus(QString,Qt::CheckState);
    void sendNeedWriteConfig();
};
#endif // AIRPRINTBACKGROUD_H
