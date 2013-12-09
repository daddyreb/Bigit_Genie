#include <QtGui>

#include "winorlinux.h"
#include "dragonnettool.h"

#include "getthread.h"


DragonNetTool::DragonNetTool()
{
    DefaultHost = "www.netgear.com";
    EnterHost = "www.netgear.com";


    DllFilePath = "./Getdetails.dll";
    DllNetwork = "./ComputerDetails.dll";
	//m_getsysinfo  = NULL;
	//m_getsysinfo = new GetSystemDetails();
//	mGetThread = new GetThread();
//	connect(mGetThread, SIGNAL(ThreadFinish()), this, SIGNAL(GetSysInfoFinish()));
    
    //about ping
    //////2011-02-21 Jansion.zhou begin
    //connect(this, SIGNAL(ResolvedSuccess()), this, SLOT(ExecutePing()));
    //connect(this, SIGNAL(ResolvedFailed()), this, SLOT(NeedNotPing()));
    ////// end

    //about traceroute
    //connect(TraceCmd, SIGNAL(readChannelFinished()), this, SLOT(ReadyTraceData()));
    connect(this, SIGNAL(ReadySuccess()), this, SLOT(ExecuteTrace()));
    connect(this, SIGNAL(ReadyFailed()), this, SLOT(NeedNotTrace()));

//    connect(&mGetThread, SIGNAL(ThreadFinish()), this, SIGNAL(GetSysInfoFinish()));

}

DragonNetTool::~DragonNetTool()
{

}

void DragonNetTool::GetSysInfoNew(QMap<QString, QMap<QString, QStringList> > &OutData)
{
//		if(m_getsysinfo)
        m_getsysinfo.SystemDetailsNew(OutData);
    emit GetSysInfoFinish();
}

void DragonNetTool::DnsLookup(const QString &HostName, QStringList* *OutData)
{
    if ( *OutData == NULL )
        *OutData = new QStringList();
    GetDataList = *OutData;
    GetDataList->clear();

    if (HostName == NULL){
        intReturn = QHostInfo::lookupHost(DefaultHost, this, SLOT(GetLookupData(QHostInfo)));
    }else {
        intReturn = QHostInfo::lookupHost(HostName, this, SLOT(GetLookupData(QHostInfo)));
    }

//    if (OutData == NULL)
//        return false;
//
//    return true;
}

int DragonNetTool::isPing(const QString &HostName)
{
    mPingNum = 4;

    if (HostName != NULL){
        EnterHost.clear();
        EnterHost = HostName;
    }else {
        EnterHost.clear();
        EnterHost = DefaultHost;
    }


    PingCmd = new QProcess();
    CmdName.clear();
    CmdName = "ping ";

    mCommand.clear();
    mCommand = CmdName + EnterHost;
    qDebug() << "The Ping Command :" << mCommand;
    PingCmd->start(mCommand);

    if ( PingCmd->waitForFinished(-1) ){

        TempData.clear();
        TempData = PingCmd->readAll();
        //qDebug() << "Ping Result Data: " << TempData;

        QStringList TempResult;
        TempResult = TempData.split(QRegExp("\\r\\n"), QString::SkipEmptyParts);
        TempData.clear();

        QRegExp SpaceMatch("\\s+");
        QString strMacth;
        strMacth = "^Ping\\s+.*"+EnterHost+".*";
        QRegExp NotFindMacth(strMacth);
        //QRegExp NotFindMacth(".*not\\s+find.*");
        //QRegExp outDataMatch("^\\s+Packets.*");
        QRegExp outDataMatch(".*(\\d+)%.*");

        QString strTempData;
        for (int i=0; i<TempResult.size(); i++){
            //qDebug() << "Row's Data: " << TempResult.value(i);
            strTempData = TempResult.value(i);

            if ( !SpaceMatch.exactMatch(strTempData) ){
                //qDebug() << "Row's Data: " << strTempData;


                if ( NotFindMacth.exactMatch(strTempData) )
                    return 0;

                if ( outDataMatch.exactMatch(strTempData) ){
                    //qDebug() << "Row's Data: " << strTempData;

                    int pos = outDataMatch.indexIn(strTempData);
                    if (pos > -1){
                        qDebug() << "Match: " << outDataMatch.cap(1);
                        QString mloss;
                        mloss = outDataMatch.cap(1);
                        if (mloss=="100"){
                            return 0;
                        }else {
                            qDebug() << "Ping Over!";
                            return 1;
                        }
                    }else {
                        return 0;
                    }

//                    QStringList OutResult;
//                    OutResult = strTempData.split(QRegExp(","), QString::SkipEmptyParts);
//                    for (int k=0; k<OutResult.size(); k++){
//                        //qDebug() << "Row's Item data: " << OutResult[k];
//                    }

                }


            }


        }

    }

    return 0;
}

void DragonNetTool::Ping(const QString &HostName, QStringList* *OutData)
{
    if ( *OutData == NULL )
        *OutData = new QStringList();
    GetDataList = *OutData;
    GetDataList->clear();

    ToolType.clear();
    ToolType = "PING";
    mPingNum = 4;

    if (HostName != NULL){
        EnterHost.clear();
        EnterHost = HostName;
    }else {
        EnterHost.clear();
        EnterHost = DefaultHost;
    }

#ifdef WIN32
    PingCmd = new QProcess();
    CmdName.clear();
    CmdName = "ping ";
#else
    PingCmd = new QProcess();
    CmdName.clear();
    CmdName = "ping -c " + mPingNum + " ";
#endif
    //connect(PingCmd, SIGNAL(readChannelFinished()), this, SLOT(ReadyPingData()));
    connect(PingCmd, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(PingDataOver(int,QProcess::ExitStatus)));
    connect(PingCmd, SIGNAL(readyRead()), this, SLOT(ReadyPingLineData()));


    //////2011-02-21 Jansion.zhou begin
    //ResolvedHost(EnterHost);
    mCommand.clear();
    mCommand = CmdName + EnterHost;
    qDebug() << "The Ping Command :" << mCommand;
    dFlag = false;
    PingCmd->start(mCommand);
    ////// end

}

void DragonNetTool::TraceRoute(const QString &HostName, QStringList* *OutData)
{
    if ( *OutData == NULL )
        *OutData = new QStringList();
    GetDataList = *OutData;
    GetDataList->clear();

    ToolType.clear();
    ToolType = "TRACE";

    if (HostName != NULL){
        EnterHost.clear();
        EnterHost = HostName;
    }else {
        EnterHost.clear();
        EnterHost = DefaultHost;
    }

#ifdef WIN32
    TraceCmd = new QProcess();
    CmdName = "tracert -d -w 1000 ";
#else
    TraceCmd = new QProcess();
    CmdName = "traceroute ";
#endif
    //connect(TraceCmd, SIGNAL(readChannelFinished()), this, SLOT(ReadyTraceData()));
    connect(TraceCmd, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(TraceDataOver(int,QProcess::ExitStatus)));
    connect(TraceCmd, SIGNAL(readyRead()), this, SLOT(ReadyTraceLineData()));

    ResolvedHost(EnterHost);

}


//std::list<std::string> DragonNetTool::splitTolist(char *para, char &mSplit)
std::list<std::string> DragonNetTool::splitTolist(std::string mString, char &mSplit)
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



void DragonNetTool::GetLookupData(const QHostInfo &Host)
{
    GetDataList->clear();

    TempData.clear();
    if (Host.error() != QHostInfo::NoError) {
        TempData = "Lookup failed:" + Host.errorString();
        GetDataList->append(TempData);
        TempData.clear();

        //qDebug() << "Lookup failed:" << host.errorString();

    }else {
        foreach (QHostAddress address, Host.addresses())
            GetDataList->append(address.toString());

        //qDebug() << "Found address:" << address.toString();
    }

    //DefaultHost.clear();
    emit DnsLookupFinish();
}

void DragonNetTool::ResolvedHost(const QString &DestHost)
{

    intReturn = QHostInfo::lookupHost(DestHost, this, SLOT(Resolved(QHostInfo)));
    //qDebug() << "DragonPing Int:" << Fresult;

}

void DragonNetTool::Resolved(const QHostInfo &Host)
{
    if (Host.error() != QHostInfo::NoError) {

        //qDebug() << "Ping DNSLookup failed:" << host.errorString();

        if (ToolType == "PING"){
            //GetDataList->append("Failed");

            QString tempStr;
            tempStr = "Ping request could not find host " + EnterHost + ".\nPlease check the name and try again.";
            GetDataList->append(tempStr);
            emit ReadPingData();

            emit ResolvedFailed();
        }else if (ToolType == "TRACE"){

            QString tempStr;
            tempStr = "Unable to resolve target system name " + EnterHost + ".";
            GetDataList->append(tempStr);
            emit ReadTraceRouteData();

            emit ReadyFailed();
        }
    }else {

        //qDebug() << "Ping DNSLookup Success!";

        if (ToolType == "PING"){
            //GetDataList->append("Success");
            emit ResolvedSuccess();
        }else if (ToolType == "TRACE"){
            emit ReadySuccess();
        }
    }
}

void DragonNetTool::NeedNotPing()
{
    //GetDataList->append("Failed");
    EnterHost.clear();
    emit PingFinish();
}

void DragonNetTool::ExecutePing()
{
    mCommand.clear();
//    mCommand = CmdName + "192.168.0.1";
    mCommand = CmdName + EnterHost;
    qDebug() << "The Ping Command :" << mCommand;
    dFlag = false;

    PingCmd->start(mCommand);

}


void DragonNetTool::ReadyPingData()
{
    TempData.clear();
    TempData = PingCmd->readAll();
    //qDebug() << "Ping Result Data: " << TempData;

    QStringList TempResult;
    TempResult = TempData.split(QRegExp("\n+"), QString::SkipEmptyParts);
    TempData.clear();

#ifdef WIN32
    QRegExp DataMatch("^\\s+Packets.*");
#else
    QRegExp DataMatch("^\\s+packets.*");
#endif
    QRegExp OutMatch(".*(\\d+)%.*loss.*");
    QStringList OutResult;

    for (int i=0; i<TempResult.size(); i++){
//        qDebug() << "Row's Data: " << TempResult.value(i);

        if ( DataMatch.exactMatch(TempResult.value(i)) ){
//            qDebug() << "Row's Data: " << TempResult.value(i);
            OutResult = TempResult.value(i).split(QRegExp(","), QString::SkipEmptyParts);
            for (int k=0; k<OutResult.size(); k++){
//                qDebug() << "Row's Item data: " << OutResult[k];
                int pos = OutMatch.indexIn(OutResult[k]);
                if (pos > -1){
                    qDebug() << "Match: " << OutMatch.cap(1);
                    QString mloss;
                    mloss = OutMatch.cap(1);
                    if (mloss=="100"){
                        GetDataList->append("Failed");
                    }else {
                        GetDataList->append("Success");
                    }

                }

            }

        }

    }

    qDebug() << "The Ping CMD Over!";
    EnterHost.clear();
    emit PingFinish();

}


void DragonNetTool::ReadyPingLineData()
{

    TempData.clear();
    TempData = PingCmd->readAll();
    QStringList TempResult;
    TempResult = TempData.split(QRegExp("\\r\\n"), QString::SkipEmptyParts);
    TempData.clear();


//#ifdef WIN32
    //QRegExp DataMatch("^(Reply|Request).*");
    QRegExp DataMatch(".*(\\d+\\.\\d+\\.\\d+\\.\\d+).*(\\d+).*(\\d+).*(\\d+).*");
    QRegExp PingMatch(".*Ping.*");
    QRegExp outMatch(".*\\d+.*");
    QRegExp SpaceMatch("\\s+");
    QString strMacth;
    strMacth = "^Ping\\s+.*"+EnterHost+".*";
    QRegExp NotFindMacth(strMacth);
    //QRegExp NotFindMacth(".*not\\s+find.*");
//#else
//    QRegExp DataMatch("^\\d+\\s+bytes.*");
//#endif
    //QRegExp OutMatch(".*(\\d+)%.*loss.*");
    //QStringList OutResult;

    //bool dFlag;
    //dFlag = false;
    QString pingData;
    QString strPing;
    QString strTemp;
    int mPos;
    for (int i=0; i<TempResult.size(); i++){
        //qDebug()<< "Number->" << i << "Row's Data: " << TempResult.value(i);
        pingData = TempResult.value(i);
        if ( !SpaceMatch.exactMatch(pingData) ){
            //qDebug()<< "Number->" << i << "Row's Data: " << TempResult.value(i);


            if ( NotFindMacth.exactMatch(pingData) ){

                QString tempStr;
                tempStr = "Ping request could not find host " + EnterHost + ". Please check the name and try again.";
                GetDataList->append(tempStr);
                //GetDataList->append(pingData);
                emit ReadPingData();


                //EnterHost.clear();
                //emit PingFinish();

            }else {

                if ( PingMatch.exactMatch(pingData) ){
                    //qDebug() << "Set Flag!" << pingData;
                    if ( dFlag )
                        dFlag = false;
                    else
                        dFlag = true;
                }else {

                    if ( dFlag ){
                        //qDebug() << "Need Data" << i << ": " << pingData;

                        mPos = DataMatch.indexIn(pingData);
                        if ( mPos > -1 ){
                            strPing = "Reply from ";
                            strTemp = DataMatch.cap(1);
                            strPing.append(strTemp);
                            strTemp.clear();
                            strPing.append(": bytes=");
                            strTemp = DataMatch.cap(2);
                            strPing.append(strTemp);
                            strTemp.clear();
                            strPing.append(" time=");
                            strTemp = DataMatch.cap(3);
                            strPing.append(strTemp);
                            strTemp.clear();
                            strPing.append("ms TTL=");
                            strTemp = DataMatch.cap(4);
                            strPing.append(strTemp);
                            strTemp.clear();

                            qDebug() << "Ping Row's Data: " << strPing;
                            GetDataList->append(strPing);
                            strPing.clear();
                            emit ReadPingData();
                        }else {
                            if ( !outMatch.exactMatch(pingData) ){
                                strPing = "Request timed out";

                                qDebug() << "Ping Row's Data: " << strPing;
                                GetDataList->append(strPing);
                                strPing.clear();
                                emit ReadPingData();
                            }else {
                                strRowData += pingData;
                                mPos = DataMatch.indexIn(strRowData);
                                if ( mPos > -1 ){
                                    strPing = "Reply from ";
                                    strTemp = DataMatch.cap(1);
                                    strPing.append(strTemp);
                                    strTemp.clear();
                                    strPing.append(": bytes=");
                                    strTemp = DataMatch.cap(2);
                                    strPing.append(strTemp);
                                    strTemp.clear();
                                    strPing.append(" time=");
                                    strTemp = DataMatch.cap(3);
                                    strPing.append(strTemp);
                                    strTemp.clear();
                                    strPing.append("ms TTL=");
                                    strTemp = DataMatch.cap(4);
                                    strPing.append(strTemp);
                                    strTemp.clear();

                                    qDebug() << "Ping Row's Data: " << strPing;
                                    GetDataList->append(strPing);
                                    strPing.clear();
                                    strRowData.clear();
                                    emit ReadPingData();
                                }
                            }
                        }

                    }
                    //else {
                    //    qDebug() << "Not Need Data!";
                    //}

                }

            }


            ///////
            //GetDataList->append(pingData);
            //emit ReadPingData();
        }

    }

}

void DragonNetTool::PingDataOver(int, QProcess::ExitStatus)
{
    EnterHost.clear();
    dFlag = false;

    QString strCmdOver;
    strCmdOver = "over";
    GetDataList->append(strCmdOver);

    emit PingFinish();
}

void DragonNetTool::NeedNotTrace()
{
    //qDebug() << "Trace Route Failed!";
    EnterHost.clear();
    emit TraceRouteFinish();
}

void DragonNetTool::ExecuteTrace()
{
    //qDebug() << "The Command :" << CmdName;
    mCommand = CmdName + EnterHost;
    //qDebug() << "The Command :" << mCommand;

	compagesData.clear();

    TraceCmd->start(mCommand);

}



void DragonNetTool::ReadyTraceLineData()
{
    /* funtion rewrite by zhouklansman 2013-06-13*/
	TempData.clear();
    TempData = QString::fromLocal8Bit(TraceCmd->readAll());

	QStringList TempResult;
	QString temstr;
    TempResult = TempData.split(QRegExp("\\r\\n"), QString::SkipEmptyParts);
	QRegExp DataMatch("^\\s+(<\\d+|\\d+|\\*)\\s+.*");
    QRegExp OutMatch("^\\s+\\d+\\s+(<\\d+|\\d+|\\*)\\s+.*\\.\\d+\\s+");
    for (int i=0; i<TempResult.size(); i++)
	{
		TempData = TempResult.at(i);
		if ( DataMatch.exactMatch(TempData))
		{
			temstr = compagesData+TempData;
			if(OutMatch.exactMatch(temstr) || temstr.count('*')>=3)
			{
				
				GetDataList->append(AddSpaceChar(temstr));
				compagesData.clear();
				emit ReadTraceRouteData();
				return;
			}
			compagesData +=TempData;
		}
		else
		{
			temstr = compagesData+TempData;
			if(OutMatch.exactMatch(temstr))
			{
				GetDataList->append(AddSpaceChar(temstr));
				compagesData.clear();
				emit ReadTraceRouteData();
			}
		}
}
	return;

 }

void DragonNetTool::TraceDataOver(int, QProcess::ExitStatus)
{
    EnterHost.clear();

    QString strCmdOver;
    strCmdOver = "over";
    GetDataList->append(strCmdOver);

    emit TraceRouteFinish();
}

QString DragonNetTool::AddSpaceChar(QString ldata)
{
    QString unitTime;
    unitTime = " ms";
    QStringList DataList;
    QString strData;
    //DataList = tempData.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    DataList = ldata.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    //qDebug()<< tempData << "The data's Row: " << DataList.size();
    QString strUnitData;
    QByteArray tempByte;
    int fRow = 3;
    int sRow = 6;
    int thRow = 6;
    int tRow = 6;
    int lRow = 3;

    //fSpaceLine = fRow-DataList[0].size();
    tempByte.fill(' ', fRow-DataList[0].size());
    strUnitData.append(tempByte);
    tempByte.clear();
    strUnitData.append(DataList[0]);
    strData.append(strUnitData);
    strUnitData.clear();
    if ( DataList[1] == "*" ){
        tempByte.fill(' ', sRow-1);
        strUnitData.append(tempByte);
        tempByte.clear();
        strUnitData.append("*");
        tempByte.fill(' ', unitTime.size());
        strUnitData.append(tempByte);
        tempByte.clear();
        strData.append(strUnitData);
        strUnitData.clear();
        if ( DataList[2] == "*" ){

            tempByte.fill(' ', thRow-1);
            strUnitData.append(tempByte);
            tempByte.clear();
            strUnitData.append("*");
            tempByte.fill(' ', unitTime.size());
            strUnitData.append(tempByte);
            tempByte.clear();
            strData.append(strUnitData);
            strUnitData.clear();

            if ( DataList[3] == "*" ){
                tempByte.fill(' ', tRow-1);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append("*");
                tempByte.fill(' ', unitTime.size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append("Request timed out");
                strData.append(strUnitData);
                strUnitData.clear();
            }else {
                tempByte.fill(' ', tRow-DataList[3].size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[3]);
                strUnitData.append(unitTime);
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[5]);
                strData.append(strUnitData);
                strUnitData.clear();
            }

        }else {
            tempByte.fill(' ', thRow-DataList[2].size());
            strUnitData.append(tempByte);
            tempByte.clear();
            strUnitData.append(DataList[2]);
            strUnitData.append(unitTime);
            strData.append(strUnitData);
            strUnitData.clear();
            if ( DataList[4] == "*" ){
                tempByte.fill(' ', tRow-1);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append("*");
                tempByte.fill(' ', unitTime.size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[5]);
                strData.append(strUnitData);
                strUnitData.clear();
            }else {
                tempByte.fill(' ', tRow-DataList[4].size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[4]);
                strUnitData.append(unitTime);
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[6]);
                strData.append(strUnitData);
                strUnitData.clear();
            }
        }

    }else {
        tempByte.fill(' ', sRow-DataList[1].size());
        strUnitData.append(tempByte);
        tempByte.clear();
        strUnitData.append(DataList[1]);
        strUnitData.append(unitTime);
        strData.append(strUnitData);
        strUnitData.clear();
        if ( DataList[3] == "*" ){
            tempByte.fill(' ', thRow-1);
            strUnitData.append(tempByte);
            tempByte.clear();
            strUnitData.append("*");
            tempByte.fill(' ', unitTime.size());
            strUnitData.append(tempByte);
            tempByte.clear();
            strData.append(strUnitData);
            strUnitData.clear();
            if ( DataList[4] == "*" ){
                tempByte.fill(' ', tRow-1);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append("*");
                tempByte.fill(' ', unitTime.size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[5]);
                strData.append(strUnitData);
                strUnitData.clear();
            }else {
                tempByte.fill(' ', tRow-DataList[4].size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[4]);
                strUnitData.append(unitTime);
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[6]);
                strData.append(strUnitData);
                strUnitData.clear();
            }


        }else {
            tempByte.fill(' ', thRow-DataList[3].size());
            strUnitData.append(tempByte);
            tempByte.clear();
            strUnitData.append(DataList[3]);
            strUnitData.append(unitTime);
            strData.append(strUnitData);
            strUnitData.clear();
            if ( DataList[5] == "*" ){
                tempByte.fill(' ', tRow-1);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append("*");
                tempByte.fill(' ', unitTime.size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[6]);
                strData.append(strUnitData);
                strUnitData.clear();
            }else {
                tempByte.fill(' ', tRow-DataList[5].size());
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[5]);
                strUnitData.append(unitTime);
                strData.append(strUnitData);
                strUnitData.clear();

                tempByte.fill(' ', lRow);
                strUnitData.append(tempByte);
                tempByte.clear();
                strUnitData.append(DataList[7]);
                strData.append(strUnitData);
                strUnitData.clear();
            }
        }
    }
    //qDebug() << "Add SpaceChar: " << strData;
    return strData;

}


