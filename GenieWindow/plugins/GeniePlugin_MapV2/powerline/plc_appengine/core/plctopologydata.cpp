#include "plctopologydata.h"
#include "plcdevice.h"
#include <QTextStream>
#include <QFile>
#include <QDesktopServices>
#include <QDir>
#include <QDateTime>
#include <QtAlgorithms>


QString PLCNodeData::toString() const
{
    QString str("(MAC:");
    str += m_devMAC;
    str += "\nIP:";
    str += m_devIP;
    str += "\nName:";
    str += m_devName;
    str += "\nModel:";
    str += m_devModel;
    str += "\nFw Version:";
    str += m_devFWVersion;
    str += "\nLED:";
    str += (m_devLEDOn ? "ON" : "OFF");
    str += "\nSecurity:";
    str += m_devNetworkType;
    str += "\nDriver:";
    str += m_devDriverPath;

    str += (m_withUSB ? "\nUSB:Support" : "\nUSB:Not Support");
    if(m_withUSB){
        str += "\nAttach USB Devices(s):[";
        int usbCount = m_attachUsbDevice.size();
        for(int idx = 0 ; idx < usbCount ; ++idx){
            str += "<";
            str += m_attachUsbDevice[idx].first;
            str += ",";
            str += m_attachUsbDevice[idx].second;
            str += ">";

            if(idx != (usbCount - 1)){
                str += " ";
            }
        }
        str += "]";
    }

    str += (m_withWireless ? "\nWireless:Support" : "\nWireless:Not Support");
    if(m_withWireless){
        str += "\nWLAN Info:";
        str += m_wlanInfo.toString();
    }

    str += "\nAttach Device(s):\n{\n";
    int attachCount = m_attachDeviceList.size();
    for(int idx = 0 ; idx < attachCount ; ++idx){
        str += m_attachDeviceList[idx].toString();
        if(idx != (attachCount - 1)){
            str += "\n";
        }
    }
    str += "}";

    str += "\nLink rate table:{";
    QStringList keys = m_devLinkRates.keys();
    QString tmpStr;
    foreach(QString key, keys){
        str += "[";
        str += key;
        str += ",";
        str += "Tx:";
        str += tmpStr.setNum(m_devLinkRates[key].first);
        str += ",";
        str += "Rx:";
        str += tmpStr.setNum(m_devLinkRates[key].second);
        str += "]";
    }
    str += "}\n";

    str += ")";

    return str;
}

PLCTopologyData::PLCTopologyData():m_routerLocated(false)
{
}

PLCTopologyData::~PLCTopologyData()
{
    while(!m_plcNodes.empty()){
        delete m_plcNodes.takeFirst();
    }
}

bool PLCTopologyData::IsRouterLocated() const
{
    return m_routerLocated;
}

QList<PLCNodeData *> PLCTopologyData::GetTopologyData() const
{
    return m_plcNodes;
}

//QList<QPair<qint16,qint16> > PLCTopologyData::GetNodeLinkRate2LocalDevice() const
//{
//    return m_node2LocalLinkRates;
//}

void PLCTopologyData::ParseScannedData(const QList<PLCDevice *> &devList, const QStringList &gatewayMacList, const QString &localDeviceMac)
{

    m_routerMacList = gatewayMacList;
    m_localDevMac = localDeviceMac;

    if(localDeviceMac.isEmpty()){
        //qDebug () << "PLC Topology data : local device not found!!!";
        //return;
    }

    PLCNodeData *nodeData = 0;
    const int devCount = devList.size();
    m_routerLocated = false;
    QString localNetworkType;


    for(int index = 0 ; index < devCount ; ++index){
        nodeData = new PLCNodeData;
        nodeData->m_devMAC = devList[index]->deviceMac();
        nodeData->m_devName = devList[index]->deviceName();
        nodeData->m_devModel = devList[index]->deviceModel();
        nodeData->m_devFWVersion = devList[index]->deviceFWVersion();
        nodeData->m_devNetworkType = devList[index]->deviceNetworkType();
        nodeData->m_devIP = devList[index]->deviceIP();
        nodeData->m_devLEDOn = devList[index]->deviceLEDState();
        nodeData->m_forwardTable = devList[index]->deviceForwardTable();
        nodeData->m_devDriverPath = devList[index]->deviceDriverPath();

        ParseAttachDevice(nodeData, devList[index]);

        nodeData->m_withUSB = devList[index]->IsUsbSupport();
        nodeData->m_attachUsbDevice = devList[index]->deviceAttachUsbDevices();

        nodeData->m_withWireless = devList[index]->IsWirelessSupport();
        nodeData->m_wlanInfo = devList[index]->deviceWLANInfo();

        bool nodeRouterAttach = false;
        if(!m_routerLocated){
            int attachDevCount = nodeData->m_attachDeviceList.count();
            for(int idx = 0 ; idx < attachDevCount ; ++idx){
                if(gatewayMacList.contains(nodeData->m_attachDeviceList[idx].m_devMac, Qt::CaseInsensitive)){
                    nodeRouterAttach = m_routerLocated = true;
                    break;
                }
            }
        }

        if(localDeviceMac == nodeData->m_devMAC){
            localNetworkType = nodeData->m_devNetworkType;
        }

        if(nodeRouterAttach){
            m_plcNodes.prepend(nodeData);
        }else{
            m_plcNodes.append(nodeData);
        }

        //device linkrates initialize
        nodeData->m_devLinkRates = devList[index]->deviceLinkRateTable();
        if(nodeData->m_devLinkRates.contains(nodeData->m_devMAC)){//DNI device
            nodeData->m_devLinkRates[localDeviceMac] = nodeData->m_devLinkRates[nodeData->m_devMAC];
            nodeData->m_devLinkRates.remove(nodeData->m_devMAC);
        }
    }

    //the network type should be identical to the local device's
    const int nodeCount = m_plcNodes.size();
    for(int idx = 0 ; idx < nodeCount ; ++idx){
        m_plcNodes[idx]->m_devNetworkType = localNetworkType;
    }

    //fix the link rates
    for(int k = 0 ; k < nodeCount ; ++k){
        for(int j = 0 ; j < nodeCount ; ++j){
            if(m_plcNodes[j]->m_devLinkRates.contains(m_plcNodes[k]->m_devMAC) &&
                    !m_plcNodes[k]->m_devLinkRates.contains(m_plcNodes[j]->m_devMAC)){
                QPair<qint16,qint16> tmpLinkRate = m_plcNodes[j]->m_devLinkRates[m_plcNodes[k]->m_devMAC];
                qSwap(tmpLinkRate.first, tmpLinkRate.second);
                m_plcNodes[k]->m_devLinkRates[m_plcNodes[j]->m_devMAC] = tmpLinkRate;
            }
        }
    }
}

void PLCTopologyData::ParseAttachDevice(PLCNodeData *node, const PLCDevice *dev)
{
    QList<AttachDeviceEntry> attachDeviceList = dev->deviceAttachDevices();
    QStringList forwardTable = dev->deviceForwardTable();
    int forwardTableSize = forwardTable.size();
    int attachDeviceSize = attachDeviceList.size();

    for(int idx = 0 ; idx < forwardTableSize ; ++idx){
        int ki = 0;
        for(ki = 0 ; ki <  attachDeviceSize ; ++ki){
            if(forwardTable[idx] == attachDeviceList[ki].m_devMac){
                node->m_attachDeviceList.append(attachDeviceList[ki]);
                break;
            }
        }

        if(ki >= attachDeviceSize){
            AttachDeviceEntry ade;
            ade.m_allowOrBlock = true;
            ade.m_devMac = forwardTable[idx];
            ade.m_isWired = true;
            ade.m_linkSpeed = 0;
            ade.m_signalStrength = 100;
            node->m_attachDeviceList.append(ade);
        }
    }

}


void PLCTopologyData::dump() const
{
    const QString dumpFileName = "plc_core_dump.txt";
    const QString netgearGenieDirName = "NETGEARGenie";
    const QString strAppDataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);


    //if(!QDir(strAppDataDir + QDir::separator() + netgearGenieDirName).exists()){
    if(!QDir(strAppDataDir).exists()){
        QDir appDataDir(strAppDataDir);
        if(!appDataDir.mkdir(netgearGenieDirName)){
            qDebug () << "PLCTopologyData dump :make dir " << netgearGenieDirName << " failed";
            return;
        }
    }

    //QFile file(strAppDataDir + QDir::separator() + netgearGenieDirName + QDir::separator() + dumpFileName);
	QFile file(strAppDataDir + QDir::separator() + dumpFileName);

    file.open(QFile::WriteOnly | QFile::Text);

    QTextStream dumpStream(&file);

    dumpStream << "<<< PLC Topoloty Data Dump >>>\n";
    dumpStream << "Date time: " << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << "\n\n";

    dumpStream << "PLC Nodes:";//(NOTE:Local device is the first node):\n";
    dumpStream << "(Local device MAC: " << m_localDevMac << ", Router MACs: ";
    foreach(QString mac , m_routerMacList){
        dumpStream << mac << " ";
    }

    dumpStream << ",Router Located: " << (m_routerLocated ? "YES" : "NO") << ")\n";

    int nodesCount = m_plcNodes.size();
    for(int idx = 0 ; idx < nodesCount ; ++idx){
        dumpStream << "Node" << (idx + 1) << ":" << m_plcNodes[idx]->toString() << "\n\n\n";
    }

    dumpStream << "\n";

    dumpStream.flush();
    file.close();
}
