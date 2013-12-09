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
	enum TrafficType //��������
	{
		AllTraffic		= 388,//�ܵ�����
		IncomingTraffic	= 264,//��������
		OutGoingTraffic	= 506 //�������
	};

	void	SetTrafficType(int trafficType);		//������������
	DWORD	GetInterfaceTotalTraffic(int index);	//�õ�index�����ӿڵ�������
        BOOL	GetNetworkInterfaceName(QString InterfaceName, int index);//�õ�����ӿ�����
	int		GetNetworkInterfacesCount();			//�õ��ӿڵ���Ŀ
	double	GetTraffic(int interfaceNumber);		//�õ�����

	DWORD	GetInterfaceBandwidth(int index);		//�õ�index�����ӿڵĴ���
        QGenie_MFNetTraffic();
        virtual ~QGenie_MFNetTraffic();
public:
	BOOL		GetInterfaces();
	double		lasttraffic;
        //QStringList Interfaces;
        QMap < int, QString >        Interfaces;
        QMap < int, DWORD >		Bandwidths;	//����
        QMap < int, DWORD >		TotalTraffics;//�ܵ�����
	int CurrentInterface;
	int CurrentTrafficType;
};

#endif // !defined(AFX_MFNETTRAFFIC_H__9CA9C41F_F929_4F26_BD1F_2B5827090494__INCLUDED_)
