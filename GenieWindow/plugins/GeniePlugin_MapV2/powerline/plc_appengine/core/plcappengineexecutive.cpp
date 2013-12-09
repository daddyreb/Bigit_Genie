#include "plcappengineexecutive.h"
//#include "mainpipeinterface.h"
#include "plcmainpipeplugin.h"
#include "plcdevicescanner.h"
#include "plctransactionmanager.h"

#include <QMutexLocker>
#include <QtDebug>

#define WAIT_TRANSACTION_TIME_MS 2000 //2 seconds

PLCAppEngineExecutive::PLCAppEngineExecutive(QObject *parent) :
    QThread(parent)
  ,m_exitFlag(false)
  ,m_mainPipeInf(0)
  ,m_deviceScanner(0)
  ,m_transactionManager(0)
  ,m_isStartupOk(false)
  ,m_newDriverFounded(false)
{
}

PLCAppEngineExecutive::~PLCAppEngineExecutive()
{
    Cleanup();
}

bool PLCAppEngineExecutive::Startup(const QString &mainPipePath, const QString &plcDriverDir)
{
    //mainPipeLoader.setFileName(mainPipePath);
    //m_mainPipeInf = qobject_cast<MainPipeInterface *>(mainPipeLoader.instance());
    m_mainPipeInf = qobject_cast<MainPipeInterface *>(new PLCMainPipePlugin());

    if(m_mainPipeInf && m_mainPipeInf->Startup(plcDriverDir,this)){
        m_exitFlag = false;
        m_newDriverFounded = false;
        start();

        m_startMutex.lock();
        while(!m_isStartupOk){
            m_startCdt.wait(&m_startMutex);
            qDebug() << "Waitting PLC App executive thread start up!!!";
        }
        m_startMutex.unlock();

        return true;
    }

    return false;
}

void PLCAppEngineExecutive::Cleanup()
{
    m_exitFlagMutex.lock();
    m_exitFlag = true;
    if(m_transactionManager){
        m_transactionManager->CancelWaitForTransaction();
    }
    m_exitFlagMutex.unlock();


    m_newDriverFounded = false;

    if(m_mainPipeInf){
        m_mainPipeInf->StopProcess();
    }

    wait();

    if(m_mainPipeInf){
        m_mainPipeInf->Cleanup();
    }

    //mainPipeLoader.unload();

    delete m_mainPipeInf;
    m_mainPipeInf = 0;
}

bool PLCAppEngineExecutive::SubmitTransaction(PLCTransaction *transaction)
{
    if(!IsExecutiveStartupOk()){
        qDebug() << "PLCAppEngineExecutive SubmitTransaction -> executive thread is not ready!!!";

        delete transaction;
        return false;
    }

    m_transactionManager->PutTransaction(transaction);

    return true;
}


void PLCAppEngineExecutive::run()
{
    m_deviceScanner = new PLCDeviceScanner(m_mainPipeInf);
    connect(m_deviceScanner, SIGNAL(PLCDeviceChangeScanned(TopologyDataTransfer))
            , this, SIGNAL(PLCDeviceChangeScanned(TopologyDataTransfer)));

    m_transactionManager = new PLCTransactionManager;
    connect(m_transactionManager, SIGNAL(TransactionFinished(TransactionPointer))
            , this, SIGNAL(TransactionFinished(TransactionPointer)));

    m_startMutex.lock();
    m_isStartupOk = true;
    m_startCdt.wakeAll();
    m_startMutex.unlock();

    Executive();

    m_startMutex.lock();
    m_isStartupOk = false;
    m_startCdt.wakeAll();
    m_startMutex.unlock();

    delete m_transactionManager;
    m_transactionManager = 0;

    delete m_deviceScanner;
    m_deviceScanner = 0;

    qDebug() << "PLCAppEngineExecutive thread exit now!!!";
}

void PLCAppEngineExecutive::Executive()
{
    //real time scanning implementation
   /* bool modifyTransactionDetected = false;
    while(!ContinueExe()){

        while((!ContinueExe()) && m_transactionManager->NeedProcessTransaction()){
            modifyTransactionDetected = m_transactionManager->ProcessTransaction(m_mainPipeInf, m_deviceScanner);
        }

        if(m_deviceScanner->IdleScanning()){
            while((!ContinueExe()) && m_transactionManager->WaitForTransaction(WAIT_TRANSACTION_TIME_MS)){
                modifyTransactionDetected = m_transactionManager->ProcessTransaction(m_mainPipeInf, m_deviceScanner);
            }
        }

        m_deviceScanner->ProcessScanning(modifyTransactionDetected || m_newDriverFounded);
        modifyTransactionDetected = false;
        m_newDriverFounded = false;

    }//while*/


    //scanning on demand implementation
    while((!ContinueExe()) && m_transactionManager->WaitForTransaction()){
        m_transactionManager->ProcessTransaction(m_mainPipeInf, m_deviceScanner);
    }
}

bool PLCAppEngineExecutive::IsExecutiveStartupOk()
{
    QMutexLocker startupLocker(&m_startMutex);
    Q_UNUSED(startupLocker);

    return m_isStartupOk;
}

bool PLCAppEngineExecutive::ContinueExe()
{
    QMutexLocker exitFlagLocker(&m_exitFlagMutex);
    Q_UNUSED(exitFlagLocker);


    return m_exitFlag;
}

void PLCAppEngineExecutive::OnNewPLCDriver(const QString &driverPath)
{
    m_newDriverFounded = true;
    qDebug () << "PLCApp engine executive receive main pipe event -> new driver: " << driverPath;
}
