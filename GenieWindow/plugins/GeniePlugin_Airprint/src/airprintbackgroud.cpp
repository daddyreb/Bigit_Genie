#include "airprintbackgroud.h"
#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QPrinterInfo>
#include <QMutexLocker>
#include <QFile>
#include "windows.h"

#define LOAD_FILE_NAME "airprintdll.dll"
#define USE_DLL_API(func) if(func) func
PrintingThread * AirprintBackgroud::printingthread = NULL;
AirprintBackgroud::AirprintBackgroud(QObject *parent)
    :QThread(parent),
    airprinterdll(NULL),
    enumPrinters(NULL),
    registerPrinter(NULL),
    unregisterPrinter(NULL),
    initdll(NULL),
    destroydll(NULL),
    exitflag(false),
    checkinstallationstatusflag(false),
    refreshprinterlistflag(false)

{
    selectedprintermap.clear();
}

AirprintBackgroud::~AirprintBackgroud()
{
    qDebug() << "AirprintBackgroud::~AirprintBackgroud enter";
    {
        QMutexLocker locker(&mutex);
        selectedprintermap.clear();
        exitflag =  true;
        checkinstallationstatusflag = false;
        refreshprinterlistflag = false;
        registerprinternamequeue.clear();
        unregisterprinternamequeue.clear();
        if(destroydll)
            destroydll();
        waitcondition.wakeOne();
    }
    qDebug() << "AirprintBackgroud::~AirprintBackgroud waiting";
    wait();
    if(printingthread)
    {
        delete printingthread;
        printingthread = NULL;
    }
    qDebug() << "AirprintBackgroud::~AirprintBackgroud end";
}

void AirprintBackgroud::enumPrinterNameList()
{
    QMap<QString,bool> tmpmap;
    QList<QString> printernamelist;
//    QList<QPrinterInfo>  printerlist =  QPrinterInfo::availablePrinters();

    ///QPrinterInfo::availablePrinters() code source
	QList<QString> printerlist;
	LPBYTE buffer;
	DWORD needed = 0;
	DWORD returned = 0;

	if ( !EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, 0, 0, &needed, &returned))
	{
		buffer = new BYTE[needed];
		if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS , NULL,
			4, buffer, needed, &needed, &returned))
		{
			delete [] buffer;
		}
		PPRINTER_INFO_4 infoList = reinterpret_cast<PPRINTER_INFO_4>(buffer);
		for (uint i = 0; i < returned; ++i) {
			printerlist.append(QString::fromWCharArray(infoList[i].pPrinterName));
		}
		delete [] buffer;
	}

    QMapIterator<QString,bool> it(selectedprintermap);
    for(int i=0; i<printerlist.count(); i++)
    {
//        QString printername = printerlist.at(i).printerName();
        QString printername = printerlist.at(i);
        printernamelist.append(printername);
        if(!selectedprintermap.contains(printername))
        {
            tmpmap.insert(printername,false);
        }
        else
        {
            tmpmap.insert(printername,selectedprintermap.value(printername));
        }
    }
    while(it.hasNext())
    {
        it.next();
        if(!printernamelist.contains(it.key()) && selectedprintermap.value(it.key()))
        {
            if(unregisterPrinter)
                unregisterPrinter(it.key().toStdString().c_str());
        }else if(printernamelist.contains(it.key()) && selectedprintermap.value(it.key()))
        {
            if(unregisterPrinter)
                unregisterPrinter(it.key().toStdString().c_str());
            if(registerPrinter)
                registerPrinter(it.key().toStdString().c_str());
        }
    }
    selectedprintermap = tmpmap;
    qDebug() << "AirprintBackgroud::enumPrinterNameList" << selectedprintermap;
    emit sendPrinterNameList(selectedprintermap);
	emit sendMessage("",false);
}

void AirprintBackgroud::setCheckInstallationState()
{
    QMutexLocker lock(&mutex);
    checkinstallationstatusflag = true;
    waitcondition.wakeOne();
}

void AirprintBackgroud::setRefreshPrinterList()
{
    QMutexLocker lock(&mutex);
    refreshprinterlistflag = true;
    waitcondition.wakeOne();
}

void AirprintBackgroud::setRegisterPrinterName(QString printername)
{
    QMutexLocker locker(&mutex);
    registerprinternamequeue.enqueue(printername);
    waitcondition.wakeOne();
}

void AirprintBackgroud::setRegisterPrinterNames(QList<QString>printernames)
{
    QMutexLocker locker(&mutex);
    for(int i =0; i< printernames.count(); i++)
    {
        registerprinternamequeue.enqueue(printernames.at(i));
    }
    waitcondition.wakeOne();
}

void AirprintBackgroud::setUnregisterPrinterName(QString printername)
{
    QMutexLocker locker(&mutex);
    unregisterprinternamequeue.enqueue(printername);
    waitcondition.wakeOne();
}

void AirprintBackgroud::setUnregisterPrinterNames(QList<QString>printernames)
{
    QMutexLocker locker(&mutex);
    for(int i =0; i< printernames.count(); i++)
    {
        unregisterprinternamequeue.enqueue(printernames.at(i));
    }
    waitcondition.wakeOne();
}

void AirprintBackgroud::registerInitPrinterNameList()
{
    QMapIterator<QString,bool> it(selectedprintermap);
    while(it.hasNext())
    {
        it.next();
        if(it.value())
            registerPrinterName(it.key());
    }
}

void AirprintBackgroud::run()
{
    QString errorstr;
    QString registerprintername;
    QString unregisterprintername;
    registerprinternamequeue.clear();
    unregisterprinternamequeue.clear();
    bool checkflag = false,
    refreshflag = false;
    status = -1;
    emit sendMessage("Initializing ...",true);
   // qDebug() << "AirprintBackgroud::run";
    if(!loadLibrary(errorstr))
    {
        emit sendMessage(errorstr,true);
        return;
    }
	printingthread = new PrintingThread();

    emit sendMessage("Checking Environment  ...",true);
    checkInstallationStatus();

    forever{
        {
            QMutexLocker locker(&mutex);
            if(registerprinternamequeue.isEmpty()
                && unregisterprinternamequeue.isEmpty()
                && !exitflag
                        && !checkinstallationstatusflag
                        && !refreshprinterlistflag)
                {
                //emit sendMessage("",false);change by qicheng.ai
                qDebug() << "waitting.....";
                waitcondition.wait(&mutex);
                qDebug() << "waitting end";
            }
            qDebug() << "running.....";
            if(exitflag)
            {
                break;
            }
            if(!registerprinternamequeue.isEmpty())
                registerprintername = registerprinternamequeue.dequeue();
            else
                registerprintername.clear();
            if(!unregisterprinternamequeue.isEmpty())
                unregisterprintername = unregisterprinternamequeue.dequeue();
            else
                unregisterprintername.clear();
            checkflag = checkinstallationstatusflag;
            checkinstallationstatusflag = false;
            refreshflag = refreshprinterlistflag;
            refreshprinterlistflag = false;
        }

        if(!registerprintername.isEmpty())
        {
            registerPrinterName(registerprintername);
        }else if(!unregisterprintername.isEmpty())
        {
            unregisterPrintername(unregisterprintername);
        }else if(checkflag)
        {
            checkInstallationStatus();
        }else if(refreshflag)
        {
            enumPrinterNameList();
        }
    }
}

bool AirprintBackgroud::loadLibrary(QString &errorstr)
{
    //qDebug() << "loadLibrary";
    //qDebug() << "currentthreadid" << QThread::currentThreadId();

    emit sendMessage("Loading Libary...",true);
    if(!QLibrary::isLibrary(LOAD_FILE_NAME))
    {
        errorstr = QString("%1 %2").arg(LOAD_FILE_NAME).arg("has not a valid suffix for a loadable library.");
   //     qDebug() << "AirprintBackgroud::loadLibrary" << errorstr;
        return false;
    }
    airprinterdll = new QLibrary;
    if(airprinterdll == NULL)
    {
        return false;
    }
    airprinterdll->setFileName(LOAD_FILE_NAME);
    if(!airprinterdll->load())
    {
        errorstr = QString("%1 %2").arg(LOAD_FILE_NAME).arg("is not loaded.");
     //   qDebug() << errorstr;
        return false;
    }else
    {
        errorstr = QString("%1 %2").arg(LOAD_FILE_NAME).arg("is loaded.");
      //  qDebug() << "AirprintBackgroud::loadLibrary" << errorstr;
    }
    enumPrinters = (pEnumPrintersFunc) airprinterdll->resolve("enumPrinters");
    registerPrinter = (pRegisterPrinter) airprinterdll->resolve("registerPrinter");
    unregisterPrinter = (pUnregisterPrinter) airprinterdll->resolve("unregisterPrinter");
    initdll = (pInit)airprinterdll->resolve("init");
    destroydll = (pDestroy)airprinterdll->resolve("destroy");
	getAdobeType = (pGetAdobeType) airprinterdll->resolve("getAdobeType");

    if(enumPrinters != NULL && registerPrinter != NULL && unregisterPrinter != NULL && initdll != NULL && destroydll != NULL)
    {
        USE_DLL_API(initdll)(getFileName);
        errorstr = QString::fromUtf8("function resolve Success");
        return true;
    }
    return false;
}

void AirprintBackgroud::registerPrinterName(QString printername)
{
    QString msg = QString("registering %1 ...").arg(printername);
    emit sendMessage(msg,true);
    if(selectedprintermap.contains(printername))
    {
        if(!selectedprintermap.value(printername))
        {
            if(registerPrinter)
            {
                registerPrinter(printername.toStdString().c_str());
                selectedprintermap.insert(printername,true);
            }
        }
    }else
    {
        if(registerPrinter)
        {
            registerPrinter(printername.toStdString().c_str());
            selectedprintermap.insert(printername,true);
        }
    }

    if(selectedprintermap.value(printername))
        emit sendPrinterStatus(printername, Qt::Checked);
    else
        emit sendPrinterStatus(printername, Qt::Unchecked);

    emit sendSaveConfig(selectedprintermap);
	emit sendMessage("",false);
}

void AirprintBackgroud::unregisterPrintername(QString printername)
{
    QString msg = QString("unregistering %1 ...").arg(printername);
    emit sendMessage(msg,true);
    if(selectedprintermap.contains(printername))
    {
        if(selectedprintermap.value(printername))
        {
            if(unregisterPrinter)
            {
                unregisterPrinter(printername.toStdString().c_str());
                selectedprintermap.insert(printername,false);
            }
        }
    }else
    {
        selectedprintermap.insert(printername,false);
    }

    if(selectedprintermap.value(printername))
        emit sendPrinterStatus(printername, Qt::Checked);
    else
        emit sendPrinterStatus(printername, Qt::Unchecked);

    emit sendSaveConfig(selectedprintermap);
	emit sendMessage("",false);
}

void AirprintBackgroud::checkInstallationStatus()
{
    emit sendMessage("Checing Installation Information ...",true);
    short currentstatus = 0x0000;
    QSettings * servicesreg = new QSettings(
            "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\Bonjour Service",
            QSettings::NativeFormat);
    QSettings * applicationreg = new QSettings(
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\AcroRd32.exe",
            QSettings::NativeFormat);

    if(servicesreg)
    {
        QVariant bonjourstartstatus = servicesreg->value("Start","0");
        qDebug() << "bonjourstartstatus" << bonjourstartstatus;
        if(!bonjourstartstatus.isNull())
        {
            bool ok;
            int value;
            value = bonjourstartstatus.toInt(&ok);
            if(ok && value == 2)
            {
                currentstatus |= ITUNESINSTALLED;
            }
        }
    }

  //  qDebug() << "currentstatus" << currentstatus;
    if(applicationreg)
    {
        QVariant adobereaderpath = applicationreg->value("Path","");
        qDebug() << "adobereaderpath" << adobereaderpath;
        if(!adobereaderpath.isNull())
        {
            QString value;
            value = adobereaderpath.toString();
            if(!value.isEmpty())
            {
                currentstatus |= ADOBEREADERINSTALLED;
				getAdobeType(1);
            }else
			{
				applicationreg = new QSettings(
					"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Acrobat.exe",
					QSettings::NativeFormat);
				if(applicationreg)
				{
					adobereaderpath = applicationreg->value("Path","");
					QString acrobatValue = adobereaderpath.toString();
					if(!acrobatValue.isEmpty())
					{
						currentstatus |= ADOBEREADERINSTALLED;
						getAdobeType(2);
					}
				}
			}
        }
    }

    qDebug() << "currentstatus" << currentstatus;
    if(status == -1)
    {
        if(currentstatus == ALLINSTALLED)
        {
            status = currentstatus;
            emit sendStatus(currentstatus);
            enumPrinterNameList();
            emit sendNeedWriteConfig();
            emit sendStatus(currentstatus);
        }else
        {
            emit sendStatus(currentstatus);
        }
    }else if(status == ALLINSTALLED)
    {
        if(currentstatus == status)
        {
            enumPrinterNameList();
        }else
        {
            status = currentstatus;
            QMapIterator<QString,bool> it(selectedprintermap);
            while(it.hasNext())
            {
                it.next();
                if(it.value())
                {
                    if(unregisterPrinter)
                        unregisterPrinter(it.key().toStdString().c_str());
                }
            }
            emit sendStatus(currentstatus);
        }
    }else
    {
        if(currentstatus == ALLINSTALLED)
        {
            status = currentstatus;
            QMapIterator<QString,bool> it(selectedprintermap);
            while(it.hasNext())
            {
                it.next();
                if(it.value())
                {
                    if(registerPrinter)
                        registerPrinter(it.key().toStdString().c_str());
                }
            }
            emit sendStatus(currentstatus);
        }else
        {
            emit sendStatus(currentstatus);
        }
    }

 //   qDebug() << "AirprintBackgroud::checkInstallationStatus" << currentstatus;
    delete servicesreg;
    delete applicationreg;
	emit sendMessage("",false);
}

void AirprintBackgroud::getFileName(const char *filename, const char *printername, int type)
{
//    qDebug() << "AirPrinterWidget::getFileName" << filename << printername << type;
    QString fn,pn;
    int t;
    t = type;
    fn = filename;
    pn = printername;
    if(printingthread)
        printingthread->setPrintingInfo(pn, fn, t);
}

PrintingThread::PrintingThread(QObject *parent)
    :QThread(parent),
    exitflag(false)
{
    start();
}

PrintingThread::~PrintingThread()
{
  //  qDebug() << "PrintingThread::~PrintingThread enter";
    {
        QMutexLocker locker(&mutex);
        printernamequeue.clear();
        filenamequeue.clear();
        typequeue.clear();
        exitflag = true;
        waitcondition.wakeOne();
    }
 //   qDebug() << "PrintingThread::~PrintingThread waiting";
    wait();
//    qDebug() << "PrintingThread::~PrintingThread exit";
}

void PrintingThread::setPrintingInfo(QString printername, QString filename, int type)
{
    QMutexLocker locker(&mutex);
    printernamequeue.enqueue(printername);
    filenamequeue.enqueue(filename);
    typequeue.enqueue(type);
    waitcondition.wakeOne();
}

void PrintingThread::run()
{
    QString printername,filename;
    int type;
	QThread::sleep(100);
    QPrinter printer;

    forever{
        {
            QMutexLocker locker(&mutex);
            if(printernamequeue.isEmpty()
                && filenamequeue.isEmpty()
                && typequeue.isEmpty()
                && !exitflag)
                {
                    qDebug() << "Printing Thread waiting...";
                    waitcondition.wait(&mutex);
                }
            if(exitflag)
            {
      //          qDebug() << "Printing Thread exiting...";
                break;
            }
        //    qDebug() << "Printing Thread running...";
            printername = printernamequeue.dequeue();
            filename = filenamequeue.dequeue();
            type = typequeue.dequeue();
         //   qDebug() << "PrintingThread::run" << printername << filename << type;
        }

//        qDebug() << "PrintingThread::run" << "sleep 10 ";
//        sleep(10);
//        qDebug() << "PrintingThread::run" << "sleep 10 end." << type;
        if(type == 1)
        {
            qDebug() << "PrintingThread::run" << "enter 1";
//            QPrinter printer;
            QSize size;
            QRect rect,rect1;
            qDebug() << "PrintingThread::run" << "enter print work 1";
            printer.setPrinterName(printername);
            qDebug() << "PrintingThread::run" << "enter print work 2";
            if(printer.isValid())
            {
                qDebug() << printername << " currently selected is a valid printer in the system";
            }else
            {
                qDebug() << printername << " currently selected is not a valid printer in the system";
                continue;
            }
            QPainter painter(&printer);
            QImage image(filename);

            if(image.height() < image.width())
            {
                QTransform transform;
                transform.translate(image.width()/2,image.height()/2);
                transform.rotate(90);
                transform.translate(-(image.height()/2),((image.width()/2) - image.height()));
                painter.setWorldTransform(transform);
                size = QSize(image.height(),image.width());
                rect1.setRect(0,0,image.height(),image.width());
            }
            else
            {
                size =  image.size();
                rect1.setRect(0,0,image.width(),image.height());
            }
            rect = painter.viewport();
            size.scale(rect.size(),Qt::KeepAspectRatio);
            painter.setViewport(rect.x(),rect.y(),
                                size.width(),size.height());
            painter.setWindow(rect1);
            painter.drawImage(0,0,image);
        }
        else if(type == 2)
        {
            qDebug() << "PrintingThread::run" << "enter 2";
            QSize size;
            QRect rect,
            rect1;

            qDebug() << "PrintingThread::run" << "enter print work 2";
//            QPrinter printer;
            qDebug() << "PrintingThread::run" << "enter print work 3";
            printer.setPrinterName(printername);
            if(printer.isValid())
                {
                    qDebug() << printername << " currently selected is a valid printer in the system";
                }else
                {
                    qDebug() << printername << " currently selected is not a valid printer in the system";
                    continue;
                }
            QPainter painter(&printer);
            QImage image(filename);
            if(image.height() < image.width())
            {
                QTransform transform;
                transform.translate(image.width()/2,image.height()/2);
                transform.rotate(90);
                transform.translate(-(image.height()/2),((image.width()/2) - image.height()));
                painter.setWorldTransform(transform);
                size = QSize(image.height(),image.width());
                rect1.setRect(0,0,image.height(),image.width());
            }
            else
            {
                size =  image.size();
                rect1.setRect(0,0,image.width(),image.height());
            }
            rect = painter.viewport();
            size.scale(rect.size(),Qt::KeepAspectRatio);
            painter.setViewport(rect.x(),rect.y(),
                                size.width(),size.height());
            painter.setWindow(rect1);
            painter.drawImage(0,0,image);
        }
        else
        {
            qDebug() << "enter else";
        }
    }
    qDebug() << "Printing Thread exit.";
}
