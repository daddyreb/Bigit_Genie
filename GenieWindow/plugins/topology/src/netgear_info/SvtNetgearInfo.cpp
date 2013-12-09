#include "SvtNetgearInfo.h"
#include "topology.h"


bool isUpgrade(QString oldV, QString newV)
{
    //////remove "V" form version
    //QRegExp vNumMatch("^V(\\d+.*\\d+)$");
    QRegExp vNumMatch("^\\D+(\\d+(\\.\\d+)*).*");
    int verpos = vNumMatch.indexIn(oldV);
    if (verpos > -1){
        oldV = vNumMatch.cap(1);
    }
    verpos = vNumMatch.indexIn(newV);
    if (verpos > -1){
        newV = vNumMatch.cap(1);
    }

    QStringList nowVersionNum = oldV.split(QRegExp("\\."), QString::SkipEmptyParts);
    QStringList newVersionNum = newV.split(QRegExp("\\."), QString::SkipEmptyParts);

    int newNum;
    int nowNum;
    QString strNum;
    bool oknew;
    bool oknow;
    int complenght;
    int nowVerL = nowVersionNum.size();
    int newVerL = newVersionNum.size();
    if ( newVerL<nowVerL ){
        complenght = newVerL;

        for (int k=nowVerL-newVerL; k>0; k--){
            if (!nowVersionNum.isEmpty())
                nowVersionNum.pop_front();
        }
    }else {
        complenght = nowVerL;

        for (int k=newVerL-nowVerL; k>0; k--){
            if (!newVersionNum.isEmpty())
                newVersionNum.pop_front();
        }
    }
    for (int i=0; i<complenght; i++){

        //////2010-10-25 jansion
        //        if (i==0){
        //            QRegExp vNumMatch("^V(\\d+)$");
        //            int newpos = vNumMatch.indexIn(newVersionNum[i]);
        //            if (newpos > -1){
        //                strNum = vNumMatch.cap(1);
        //                newNum = strNum.toInt(&oknew, 10);
        //            }
        //            int nowpos = vNumMatch.indexIn(nowVersionNum[i]);
        //            if (nowpos > -1){
        //                strNum = vNumMatch.cap(1);
        //                nowNum = strNum.toInt(&oknow, 10);
        //            }
        //
        //            if (oknew && oknow){
        //                if (newNum>nowNum)
        //                    return true;
        //            }
        //        }else {
        //            strNum = newVersionNum[i];
        //            newNum = strNum.toInt(&oknew, 10);
        //            strNum = nowVersionNum[i];
        //            nowNum = strNum.toInt(&oknow, 10);
        //
        //            if (oknew && oknow){
        //                if (newNum>nowNum)
        //                    return true;
        //            }
        //        }
        //////



        strNum = newVersionNum[i];
        newNum = strNum.toInt(&oknew, 10);
        strNum = nowVersionNum[i];
        nowNum = strNum.toInt(&oknow, 10);

        if (oknew && oknow){
            if (newNum>nowNum){
                return true;
            }
        }

    }

    return false;

}

////////////////////////////////////////////////
SVT_NetgearInfoThread::SVT_NetgearInfoThread()
{
}


void SVT_NetgearInfoThread::run()
{
    qDebug()<<"SVT_NetgearInfoThread::run()";
    //    SVT_Core &coreRef=SVT_Core::GetSingleton();
    QMap<int,QString> currentsetting_map;
    QMap<int,QString> update_map;
    SVT_ControlProxy *control_proxy=SVT_Core::GetSingleton().mControlInterface;

    QHostInfo hinfo = QHostInfo::fromName("routerlogin.net");
    QHostAddress  routerAddr;
    if(hinfo.error() == QHostInfo::NoError && !hinfo.addresses().isEmpty())
    {
        QList<QHostAddress> addrList = hinfo.addresses();
        qDebug() << "routerlogin.net ips = " << addrList;
        foreach(QHostAddress  addr ,addrList)
        {
            if(addr.protocol() == QAbstractSocket::IPv4Protocol)
            {
                routerAddr = addr;
                qDebug () << "routerAddr = " << routerAddr;
                break;
            }
        }
    }
    else
    {
        qDebug () << "SVT_NetgearInfoThread :hinfoerror = " << hinfo.errorString()
                << "addr list =  " << hinfo.addresses();
    }

    if (!routerAddr.isNull())
    {
        SVT_IpAddress tempip;
        tempip.Set(routerAddr);
        CORE_UTIL().netgearGetCurrentSetting(tempip.asLong(),currentsetting_map);

        //get routelogin.net 's mac start
        SVT_IpAddress srcip;
        QList<SVT_NetworkInterface> ifaces;
        CORE_UTIL().netscanGetAllAdpInfo(ifaces);
        QList<SVT_NetworkInterface>::iterator i;
        for (i = ifaces.begin(); i != ifaces.end(); ++i)
        {
            const quint32 mask=(*i).getMask().asLong();
            if( ((*i).getIp().asLong()&mask) == (tempip.asLong()&mask) )
            {
                srcip=(*i).getIp();
            }
        }
        bool btemp;
        SVT_MacAddress mac=CORE_UTIL().netscanGetMacForRouter(tempip,srcip,&btemp);
        if(btemp)
        {
            currentsetting_map.insert(NIE_CURRENTSETTING_ROUTELOGINDOTNETMAC,mac.toString());
        }
        //get routelogin.net 's mac end
        currentsetting_map.insert(NIE_CURRENTSETTING_MESSAGETYPE,"currentsetting");
        currentsetting_map.insert(NIE_CURRENTSETTING_ROUTELOGINDOTNETIP,routerAddr.toString());
        control_proxy->funcNetgearInfoReturned(currentsetting_map);

        qDebug () << "SVT_NetgearInfoThread :currentsetting_map = " << currentsetting_map;

        QString model=currentsetting_map.value(NIE_CURRENTSETTING_MODEL);
        QString regiontag=currentsetting_map.value(NIE_CURRENTSETTING_REGION,"ww");
        QString firmware=currentsetting_map.value(NIE_CURRENTSETTING_FIRMWARE,"");

        if(!model.isEmpty()&& !firmware.isEmpty())
        {
            QString para=QString("/%1/%2/").arg(model,regiontag);

            CORE_UTIL().netgearGetFirmwareupdateInfo(para,update_map);
            if(update_map.contains(NIE_UPDATE1_VERSION))
            {
                update_map.insert(NIE_UPDATE1_NEEDUPDATE,
                                  (firmware!="" && isUpgrade(firmware,update_map[NIE_UPDATE1_VERSION]))?"y":"n"
                                  );

            }
            update_map.insert(NIE_CURRENTSETTING_MESSAGETYPE,"update");
            control_proxy->funcNetgearInfoReturned(update_map);

        }
    }
    else
    {
        currentsetting_map.insert(NIE_CURRENTSETTING_MESSAGETYPE,"currentsetting");
        control_proxy->funcNetgearInfoReturned(currentsetting_map);
    }

}
