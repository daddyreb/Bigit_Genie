#include "plcdevicesoapthread.h"
#include "plcdevice.h"
#include "mainpipeinterface.h"

PLCDeviceSoapThread::PLCDeviceSoapThread(QObject *parent) :
    QThread(parent)
  ,m_device(0)
  ,m_mainPipeInf(0)
{
}

PLCDeviceSoapThread::~PLCDeviceSoapThread()
{
    wait();
}

void PLCDeviceSoapThread::run()
{
    m_device->ScanAttachDevice(m_mainPipeInf);
    m_device->ScanAttachUsbDevice(m_mainPipeInf);
    m_device->ScanWLANInfo(m_mainPipeInf);
}

void PLCDeviceSoapThread::begin(PLCDevice *device, MainPipeInterface *mainPipeInf)
{
    m_device = device;
    m_mainPipeInf = mainPipeInf;

    start();
}
