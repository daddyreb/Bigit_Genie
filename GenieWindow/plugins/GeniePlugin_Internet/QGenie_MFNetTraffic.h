// QGenie_MFNetTraffic.h: interface for the QGenie_MFNetTraffic class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MFNETTRAFFIC_H__9CA9C41F_F929_4F26_BD1F_2B5827090494__INCLUDED_)
#define AFX_MFNETTRAFFIC_H__9CA9C41F_F929_4F26_BD1F_2B5827090494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxtempl.h>
#include "windows.h"
#include <QString>
#include <QStringList>
#include <QMap>
//#include "afxres.h"
#include "limits.h"


class QGenie_MFNetTraffic
{
public:
	enum TrafficType //流量类型
	{
		AllTraffic		= 388,//总的流量
		IncomingTraffic	= 264,//输入流量
		OutGoingTraffic	= 506 //输出流量
	};

	void	SetTrafficType(int trafficType);		//设置流量类型
	DWORD	GetInterfaceTotalTraffic(int index);	//得到index索引接口的总流量
        BOOL	GetNetworkInterfaceName(QString InterfaceName, int index);//得到网络接口名字
	int		GetNetworkInterfacesCount();			//得到接口的数目
	double	GetTraffic(int interfaceNumber);		//得到流量

	DWORD	GetInterfaceBandwidth(int index);		//得到index索引接口的带宽
        QGenie_MFNetTraffic();
        virtual ~QGenie_MFNetTraffic();
public:
	BOOL		GetInterfaces();
	double		lasttraffic;
        //QStringList Interfaces;
        QMap < int, QString >        Interfaces;
        QMap < int, DWORD >		Bandwidths;	//带宽
        QMap < int, DWORD >		TotalTraffics;//总的流量
	int CurrentInterface;
	int CurrentTrafficType;
};

#endif // !defined(AFX_MFNETTRAFFIC_H__9CA9C41F_F929_4F26_BD1F_2B5827090494__INCLUDED_)
