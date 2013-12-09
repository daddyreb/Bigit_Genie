#include <QtGui>
#include <QMessageBox>

#include "getsystemdetails.h"
#include "winorlinux.h"


GetSystemDetails::GetSystemDetails()
{
    mdllPath = QString("./ComputerDetails.dll");
}

GetSystemDetails::~GetSystemDetails()
{

}

void GetSystemDetails::SystemDetailsNew(QMap<QString, QMap<QString, QStringList> > &OutData)
{
	GetNetworkInfo(OutData);
	GetOperationInfo_sys(OutData);
	emit GetDetailsFinish();
}

void GetSystemDetails::GetOperationInfo_sys(QMap<QString, QMap<QString, QStringList> > &OutData)
{

	if(mdllPath.compare("") ==0)
	{
		QMessageBox::warning(NULL,"exit",mdllPath);
		return;
	}
    qDebug() << "Load Operation is start" << mdllPath;
    sysDllLibrary = new QLibrary(mdllPath);
    if ( sysDllLibrary->load() ){


        typedef BOOL (*GetFunction)(INFOLIST**);
        GetFunction GetOperationDetails = (GetFunction) sysDllLibrary->resolve("GetOperationDetails");

        typedef void (*FreeFunction)(INFOLIST*);
        FreeFunction FreeOperListBuff = (FreeFunction) sysDllLibrary->resolve("FreeOperListBuff");


        QString namePara;
        namePara = "Operating System";

        INFOLIST* rListData = NULL;
        GetOperationDetails(&rListData);
       
		qDebug() << "Get result Data!" << rListData->m_Count;

        ////Get return result Data
        mDetails.clear();
        QString ItemName;
        QString ItemValue;
        cDetailsMap.clear();

        for (int i=0; i<rListData->m_Count; i++){
            ItemName.clear();
            ItemValue.clear();

            qDebug() << "Data: " << QString::fromWCharArray(rListData->lpItem[i].m_Name)
                     << "-->" << QString::fromWCharArray(rListData->lpItem[i].m_Value);

            ItemName = QString::fromWCharArray(rListData->lpItem[i].m_Name);
            ItemValue = QString::fromWCharArray(rListData->lpItem[i].m_Value);

            mDetails.push_back(ItemName);
            mDetails.push_back(ItemValue);

        }

        cDetailsMap.insert("OpertionInfo", mDetails);
        OutData.insert(namePara, cDetailsMap);


        //ÊÍ·ÅÄÚ´æ
        FreeOperListBuff(rListData);
        sysDllLibrary->unload();

    }else {
        qDebug() << "Load " << mdllPath << " is failed";

        mDetails.clear();
        mDetails.push_back("Load the ComputerDetails.dll is failed");
        cDetailsMap.clear();
        cDetailsMap.insert("Failed", mDetails);
        OutData.insert("Load Error", cDetailsMap);
        cDetailsMap.clear();
    }


}

void GetSystemDetails::GetNetworkInfo(QMap<QString, QMap<QString, QStringList> > &OutData)
{
	 try
	 {
	QStringList netDetails;
    qDebug() << "Load Get Network Interface" <<mdllPath ;
    QMap<QString, QStringList> netDetailsMap;
    netDllLibrary = new QLibrary(mdllPath);
    if ( netDllLibrary->load() ){
        qDebug() << "Load " << mdllPath << " is succ";

        typedef bool (*GetFunction)(NICLIST **);
        GetFunction NetworkInterfaces = (GetFunction) netDllLibrary->resolve("NetworkInterfaces");

        typedef void (*FreeFunction)(NICLIST*);
        FreeFunction FreeNicListBuff = (FreeFunction) netDllLibrary->resolve("FreeNicListBuff");

		if(!NetworkInterfaces)
		{
			qDebug() << "find NetworkInterfaces error!";
			return;
		}

        NICLIST *niclist = NULL;
		bool ret =   NetworkInterfaces(&niclist);

		netDetailsMap.clear();
        int nType;
        QString strId;
        //QString strType;
        QString strGuid;
        QList<QNetworkAddressEntry> ipInfoList;

        //niclist = NULL;
        if ( niclist != NULL ){

            for (DWORD i=0;i<niclist->dwCount;++i)
            {
                nType = int(niclist->lpNics[i].m_dwNicType);
                if ( nType==1 || nType==2 ){
                    netDetails.clear();
                    strId = QString::number(int(niclist->lpNics[i].m_dwId), 10);
                    netDetails.push_back("Id");
                    netDetails.push_back(strId);
 
                    strGuid = QString::fromWCharArray(niclist->lpNics[i].m_chDevGuid);
 

                    //Add other Info
                    otherDetails = QNetworkInterface::interfaceFromName(strGuid);
                    netDetails.push_back("Name");
                    netDetails.push_back(otherDetails.humanReadableName());
                    netDetails.push_back("HardwareAddress");
                    netDetails.push_back(otherDetails.hardwareAddress());

                    ipInfoList = otherDetails.addressEntries();
                    foreach(QNetworkAddressEntry ipInfo, ipInfoList){
                        netDetails.push_back("IP Address");
                        netDetails.push_back(ipInfo.ip().toString());
                        netDetails.push_back("Netmask");
                        netDetails.push_back(ipInfo.netmask().toString());
                        netDetails.push_back("Broadcast");
                        netDetails.push_back(ipInfo.broadcast().toString());

                    }

                    netDetails.push_back("Description");
                    netDetails.push_back(QString::fromWCharArray(niclist->lpNics[i].m_chDevDesc));

                    netDetailsMap.insert(strId, netDetails);
                }

            }

            OutData.insert("Network Interface", netDetailsMap);
            netDetailsMap.clear();

            FreeNicListBuff(niclist);

        }

        netDllLibrary->unload();

    }else {
        qDebug() << "Load " << mdllPath << " is failed";
        netDetails.clear();
        netDetails.push_back("Load the ComputerDetails.dll is failed");
        netDetailsMap.clear();
        netDetailsMap.insert("Failed", netDetails);
        OutData.insert("Load Error", netDetailsMap);
        netDetailsMap.clear();
    }
	}
	 catch (...)
	 {
		 qDebug() << "Load Get Network Interface error" << mdllPath ;

	 }
}

std::list<std::string> GetSystemDetails::StringSplit(std::string mString, char &mSplit)
{

//    std::string mString = para;
    qDebug() << "Split Para: " << mString.c_str();

    std::list<std::string> relist;

    std::string::size_type pos1, pos2;
    pos2 = mString.find(mSplit);
    pos1 = 0;
    while (std::string::npos != pos2){
        relist.push_back(mString.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = mString.find(mSplit, pos1);
    }

    relist.push_back(mString.substr(pos1));

    return relist;
}



