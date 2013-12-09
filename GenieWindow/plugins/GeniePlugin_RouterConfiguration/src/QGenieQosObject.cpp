#include "QGenieQosObject.h"
#include <QUiLoader>
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "QGenieTimelineLocker.h"
QGenieQosObject::QGenieQosObject(QObject *parent)
	:QObject(parent)
	,mW(NULL)
	,mFlag(-1)
{
	/////init m_AplicationItems and m_OnlineGameItems
	m_AplicationItems<<"MSN Messenger"<<"Skype"<<"Yahoo Messenger"<<"IP Phone"<<"Vonage IP Phone"<<"NetMeeting"<<"AIM"<<"Google Talk"
		<<"Netgear EVA"<<"SSH"<<"Telnet"<<"VPN"<< "FTP"<<"SMTP"<<"WWW"<<"DNS"
		<<"ICMP"<<"eMule / eDonkey"<<"Kazaa"<<"Gnutella"<<"BT / Azureus"<<"Xbox"<<"Add a new application";
	m_OnlineGameItems<<"Counter Strike"<<"Age of Empires"<<"Everquest"<<"Quake 2"<<"quake 3"<<"Unreal Tournament"<<"Warcraft"<<"Add a new game";
}

QGenieQosObject::~QGenieQosObject()
{
}

QWidget* QGenieQosObject::getQosUi()
{
	if(!mW)
	{
		bool btemp;
		QUiLoader loader;
		QFile f(GENIE2_RES("ui/QosSetting.ui"));
		btemp = f.open(QFile::ReadOnly);
		Q_ASSERT(btemp);
		QWidget *w = loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
		Q_ASSERT(w);
		w->hide();
		QList<QPushButton *> bts=w->findChildren<QPushButton *>();
		int c=bts.count();
		for(int i=0;i<c;i++)
		{
			connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
		}
		chk_QosSwitch = w->findChild<QCheckBox*>("ckb_QosSwitch");
		stackMain_Qos = w->findChild<QStackedWidget*>("stackedWidget_Main");
		stackSecond_Qos = w->findChild<QStackedWidget*>("stackedWidget_Second");
		stackThird_Qos = w->findChild<QStackedWidget*>("stackedWidget_Third");

		cbx_PriorityCategory = w->findChild<QComboBox*>("cbx_PriorityCategory");
		cbx_Priority = w->findChild<QComboBox*>("cbx_Priority");
		cbx_ConnectionType = w->findChild<QComboBox*>("cbx_ConnectionType");
		cbx_Applictaions = w->findChild<QComboBox*>("cbx_Applictaions");

		lab_Applictions = w->findChild<QLabel*>("lab_Applictions");
		tableView_RuleList = w->findChild<QTableView*>("tableView_RuleList");
		lineEdit_QoSPolicy = w->findChild<QLineEdit*>("lineEdit_QoSPolicy");
		lineEdit_StartPort = w->findChild<QLineEdit*>("lineEdit_StartPort");
		lineEdit_EndPort = w->findChild<QLineEdit*>("lineEdit_EndPort");
		lineEdit_DeviceName = w->findChild<QLineEdit*>("lineEdit_DeviceName");


		Q_ASSERT(chk_QosSwitch && stackMain_Qos && stackSecond_Qos && stackThird_Qos);
		QList<QComboBox *> ckb = w->findChildren<QComboBox *>();
		int ckbCount = ckb.count();
		for (int i = 0;i<ckbCount;i++)
		{
			connect(ckb[i],SIGNAL(currentIndexChanged(int)),this,SLOT(slot_combxIndexChanged(int)));
		}
		connect(chk_QosSwitch,SIGNAL(stateChanged(int)),this,SLOT(slot_ckbStateChanged(int)));
		mW = w;

		QStringList itemList;
		itemList<<"Applications"<<"Online Gaming"<<"Ethernet LAN Port"<<"MAC Address";
		cbx_PriorityCategory->addItems(itemList);
		itemList.clear();
		itemList<<"Highest"<<"High"<<"Normal"<<"Low";
		cbx_Priority->addItems(itemList);
		itemList.clear();
		itemList<<"TCP/UDP"<<"TCP"<<"UDP";
		cbx_ConnectionType->addItems(itemList);
		
		///init tableview data
		refleshRulesList();
	}
	return mW;
}


////translate ui
void QGenieQosObject::retranslateUi()
{

}

///all QPushButton event
void QGenieQosObject::slot_buttonClicked()
{
	QPushButton *but=qobject_cast<QPushButton *>(sender());
	Q_ASSERT(but);
	QString objname=but->objectName();
	if(objname == "btn_PriorityRuleAdd")
	{
		mFlag = 0;
		stackMain_Qos->setCurrentIndex(1);
		stackThird_Qos->setVisible(true);
		cbx_PriorityCategory->setEnabled(true);
		lineEdit_QoSPolicy->setEnabled(true);
		cbx_Applictaions->setEnabled(true);
		lineEdit_QoSPolicy->clear();
		cbx_PriorityCategory->setCurrentIndex(0);
		int index;
		index= cbx_Applictaions->count();
		cbx_Applictaions->setCurrentIndex(index-1);
		cbx_Priority->setCurrentIndex(2);//0:highest,1:high,2:nomal,3:low;
	}else if(objname == "btn_Cancel")
	{
		stackMain_Qos->setCurrentIndex(0);
	}else if(objname == "btn_Apply")
	{
		if(0 == mFlag)
		{
			int index;
			index = cbx_PriorityCategory->currentIndex();
			if( 0 == index || 1 == index)
			{
				QStringList paramList;
				QString qosPolicy = lineEdit_QoSPolicy->text().trimmed();
				QString priority  = getPrority(cbx_Priority->currentIndex());
				QString connTyep = getConnectionType(cbx_ConnectionType->currentIndex());
				QString startPort = lineEdit_StartPort->text().trimmed();
				QString endPort = lineEdit_EndPort->text().trimmed();
				paramList<< qosPolicy <<"Applications"<< priority << connTyep << startPort << endPort;
				QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
					->netgearCommand(INetgearInterface::IC_ADD_QOSRULE,paramList,this,SLOT(slot_DataReturned(int,QStringList)));

			}else if(2 == index)
			{
				QStringList paramList;
				QString qosPolicy = lineEdit_QoSPolicy->text().trimmed();
				QString priority  = getPrority(cbx_Priority->currentIndex());
				paramList<< qosPolicy << priority << cbx_Applictaions->currentText();
				QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
					->netgearCommand(INetgearInterface::IC_ADD_QOSRULEBYETHERNETPORT,paramList,this,SLOT(slot_DataReturned(int,QStringList)));
			}else if (3 == index)
			{
				QStringList paramList;
				QString qosPolicy = lineEdit_QoSPolicy->text().trimmed();
				QString priority  = getPrority(cbx_Priority->currentIndex());
				QString macAddress = getMacAddress();
				QString deviceName = lineEdit_DeviceName->text().trimmed();
				paramList<< qosPolicy << priority << macAddress << deviceName;
				QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
					->netgearCommand(INetgearInterface::IC_ADD_QOSRULEBYMAC,paramList,this,SLOT(slot_DataReturned(int,QStringList)));
			}
		}else if(1 == mFlag)
		{
			QStringList paramList;
			int index;
			index = cbx_Priority->currentIndex();
			QString tempPriority = getPrority(index);
			
			paramList<< lineEdit_QoSPolicy->text().trimmed() << tempPriority;
			QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
				->netgearCommand(INetgearInterface::IC_UPDATE_QOSPRIORITY,paramList,this,SLOT(slot_DataReturned(int,QStringList)));
		}
		stackMain_Qos->setCurrentIndex(0);

	}else if("btn_PriorityRuleDel" == objname)
	{
		QStringList paramList;
		QString ruleName = getSelectRowData();
		if(ruleName!="")
		{
			paramList<<ruleName;
			QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
				->netgearCommand(INetgearInterface::IC_DELETE_QOSRULE,paramList,this,SLOT(slot_DataReturned(int,QStringList)));
		}
		
	}else if("btn_PriorityRuleRefresh" == objname)
	{
		refleshRulesList();
	}else if("btn_PriorityRuleEdit" == objname)
	{
		mFlag = 1;
		stackMain_Qos->setCurrentIndex(1);
		stackThird_Qos->setVisible(false);
		cbx_PriorityCategory->setEnabled(false);
		lineEdit_QoSPolicy->setEnabled(false);
		cbx_Applictaions->setEnabled(false);

		QString tempRuleName;
		tempRuleName = getSelectRowData();

		lineEdit_QoSPolicy->setText(tempRuleName);
		QStringList rowList;
		rowList = m_ruleList.value(tempRuleName,QStringList());
		if(rowList.count() > 0)
		{
			if(rowList[1].compare("Applications",Qt::CaseInsensitive) == 0)
			{
				int indexApp;
				cbx_PriorityCategory->setCurrentIndex(0);
				indexApp = m_AplicationItems.indexOf(tempRuleName);
				if(indexApp > 0)
				{
					cbx_Applictaions->setCurrentIndex(indexApp);
				}
				else
				{
					cbx_Applictaions->setCurrentIndex(m_AplicationItems.count()-1);
				}
				

			}else if(rowList[1].compare("Online Gamming",Qt::CaseInsensitive) == 0)
			{
				cbx_PriorityCategory->setCurrentIndex(1);
				int indexOnlineGame;
				indexOnlineGame = m_OnlineGameItems.indexOf(tempRuleName);
				if(indexOnlineGame > 0)
				{
					cbx_Applictaions->setCurrentIndex(indexOnlineGame);
				}
				else
				{
					cbx_Applictaions->setCurrentIndex(m_OnlineGameItems.count()-1);
				}
			}else if(rowList[1].compare("Ethernet LAN Port",Qt::CaseInsensitive) == 0)
			{
				cbx_PriorityCategory->setCurrentIndex(2);
				bool btemp;
				int itemp;
				itemp = rowList[4].toInt(&btemp);
				if(btemp)
				{
					cbx_Applictaions->setCurrentIndex(itemp-1);
				}

			}else if(rowList[1].compare("MAC Address",Qt::CaseInsensitive) == 0)
			{
				cbx_PriorityCategory->setCurrentIndex(3);
			}

			if(rowList[2].compare("Highest",Qt::CaseInsensitive) == 0)
			{
				cbx_Priority->setCurrentIndex(0);
			}else if(rowList[2].compare("High",Qt::CaseInsensitive) == 0)
			{
				cbx_Priority->setCurrentIndex(1);
			}else if(rowList[2].compare("Normal",Qt::CaseInsensitive) == 0)
			{
				cbx_Priority->setCurrentIndex(2);
			}else if(rowList[2].compare("Low",Qt::CaseInsensitive) == 0)
			{
				cbx_Priority->setCurrentIndex(3);
			}
		}
	}
	
}

void QGenieQosObject::slot_ckbStateChanged(int state)
{
	if(state == Qt::Unchecked)
	{
		QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
			->netgearCommand(INetgearInterface::IC_SET_QOSENABLESTATUS,QStringList()<<"0",NULL,NULL);
	}else if (state == Qt::Checked)
	{
		QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
			->netgearCommand(INetgearInterface::IC_SET_QOSENABLESTATUS,QStringList()<<"1",NULL,NULL);
	}
}

//////////////////// all QCombobox IndexChanged(int) slot////////////////////////////////////////
void QGenieQosObject::slot_combxIndexChanged(int index)
{
	QComboBox *cbx=qobject_cast<QComboBox *>(sender());
	Q_ASSERT(cbx);
	QString objname=cbx->objectName();
	if(objname == "cbx_PriorityCategory")
	{
		QStringList itemList;
		cbx_Applictaions->clear();
		if(0 == index)
		{
			lab_Applictions->setText("Applications");
			stackSecond_Qos->setCurrentIndex(0);
			itemList<<"MSN Messenger"<<"Skype"<<"Yahoo Messenger"<<"IP Phone"<<"Vonage IP Phone"<<"NetMeeting"<<"AIM"<<"Google Talk"
				<<"Netgear EVA"<<"SSH"<<"Telnet"<<"VPN"<< "FTP"<<"SMTP"<<"WWW"<<"DNS"
				<<"ICMP"<<"eMule / eDonkey"<<"Kazaa"<<"Gnutella"<<"BT / Azureus"<<"Xbox"<<"Add a new application";
			cbx_Applictaions->addItems(itemList);
			cbx_Applictaions->setCurrentIndex(itemList.count()-1);
			if(0 == mFlag)
			{
				stackThird_Qos->setVisible(true);
				stackThird_Qos->setCurrentIndex(0);
			}else if(1 == mFlag)
			{
				stackThird_Qos->setVisible(false);
				//stackThird_Qos->setCurrentIndex(0);
			}
			
		}else if(1 == index)
		{
			lab_Applictions->setText("Online Gaming");
			stackSecond_Qos->setCurrentIndex(0);
			itemList<<"Counter Strike"<<"Age of Empires"<<"Everquest"<<"Quake 2"<<"quake 3"<<"Unreal Tournament"<<"Warcraft"<<"Add a new game";
			cbx_Applictaions->addItems(itemList);
			cbx_Applictaions->setCurrentIndex(itemList.count()-1);
			if(0 == mFlag)
			{
				stackThird_Qos->setVisible(true);
				stackThird_Qos->setCurrentIndex(0);
			}else
			{
				stackThird_Qos->setVisible(false);
			}
			
		}else if(index == 2)
		{
			lab_Applictions->setText("Ethernet LAN Port");
			stackSecond_Qos->setCurrentIndex(0);
			itemList <<"1"<<"2"<<"3"<<"4";
			cbx_Applictaions->addItems(itemList);
			stackThird_Qos->setVisible(false);
		}else if(index == 3)
		{
			stackThird_Qos->setVisible(false);
			stackSecond_Qos->setCurrentIndex(1);
			//stackThird_Qos->setCurrentIndex(1);
		}
	}else if("cbx_Applictaions" == objname)
	{
		int appCount = cbx_Applictaions->count();
		int index = cbx_Applictaions->currentIndex();
		int indexCbxCategory = cbx_PriorityCategory->currentIndex();
		if(mFlag == 0)
		{
			if( 2 == indexCbxCategory)
			{
				QString priority = "Lan port " + cbx_Applictaions->currentText();
				lineEdit_QoSPolicy->setText(priority);
			}else
			{
				if(index!= (appCount-1))
				{
					lineEdit_QoSPolicy->setText(cbx_Applictaions->itemText(index));
				}else
				{
					lineEdit_QoSPolicy->setText("");
				}
			}
			
		}
	}
}

///--------init some QComboBox items
void QGenieQosObject::initComboxItem(COMBOXTYPE_ENUM comboxType)
{
	QStringList itemList;
	if(comboxType == PRIORITY_CATEGORY)
	{
		itemList<<"Applications"<<"Online Gaming"<<"Ethernet LAN Port"<<"MAC Address";
		cbx_PriorityCategory->addItems(itemList);
	}else if(comboxType == PRIORITY)
	{
		itemList<<"Highest"<<"High"<<"Normal"<<"Low";
		cbx_Priority->addItems(itemList);
	}else if(comboxType == CONNECTIONTYPE)
	{
		itemList<<"TCP/UDP"<<"TCP"<<"UDP";
		cbx_ConnectionType->addItems(itemList);
	}

}
void QGenieQosObject::slot_DataReturned(int session,const QStringList &lst)
{
	if(lst.count()>0)
	{

	}
	refleshRulesList();
}

////---------reflesh RulesList---------------
void QGenieQosObject::refleshRulesList()
{
	QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
		->netgearCommand(INetgearInterface::IC_GET_QOSRULES,QStringList(),this,SLOT(slot_GetRulesDataReturned(int,QStringList)));
}

void QGenieQosObject::slot_GetRulesDataReturned(int session,const QStringList &lst)
{
	bool btemp;
	int length=lst.count();
	bool requestOk=(length>0 && (0==lst[0].toInt(&btemp)) && btemp);
	if(requestOk)
	{
		QString stemp = lst[1];
		if(stemp.isEmpty())
		{
			return;
		}else
		{
			QRegExp reg("@\\d+;");
			QStringList slist=stemp.split(reg);
			int c=slist.count();
			if(c<1)
			{
				return;
			}
			else
			{
				QStandardItemModel *model = new QStandardItemModel(); 
				tableView_RuleList->setModel(model);
				QStringList header;
				header<<"QoS Policy"<<"Priority Category"<<"Priority";
				model->setHorizontalHeaderLabels(header);
				model->horizontalHeaderItem(0)->setBackground(QColor(60,60,60));
				tableView_RuleList->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
				tableView_RuleList->setSelectionBehavior(QAbstractItemView::SelectRows);
				tableView_RuleList->setEditTriggers(QAbstractItemView::NoEditTriggers);
				tableView_RuleList->setSelectionMode(QAbstractItemView::SingleSelection);
				tableView_RuleList->resizeColumnsToContents();

				QHeaderView * headerView = tableView_RuleList->horizontalHeader();
				headerView->resizeSection(0,240);
				headerView->resizeSection(1,170);
				tableView_RuleList->horizontalHeader()->setStretchLastSection(true);
				int rows = slist[0].toInt();
				m_ruleList.clear();
				for(int i=1;i<c;i++)
				{
					QStringList ssublist=slist[i].split(";");
					int cc=ssublist.count();
					QStandardItem* firstCol = new QStandardItem(ssublist[0]);
					QStandardItem* secondCol = new QStandardItem(ssublist[1]);
					QStandardItem* thirdCol = new QStandardItem(ssublist[2]);
					firstCol->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					secondCol->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					thirdCol->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					model->setItem(i-1,0,firstCol);
					model->setItem(i-1,1,secondCol);
					model->setItem(i-1,2,thirdCol);
					
					m_ruleList.insert(ssublist[0],ssublist);
				}
				tableView_RuleList->selectRow(0);
			}
		}
	}
	
}

////-------get selection rules name int tableview-------
QString QGenieQosObject::getSelectRowData()
{
	int curRow;
	QModelIndex modelIndex = tableView_RuleList->currentIndex();
	if(!modelIndex.isValid())
	{
		return "";
	}
	curRow = modelIndex.row();
	QAbstractItemModel* model = tableView_RuleList->model();
	modelIndex = model->index(curRow,0);
	QString tempRuleName;
	tempRuleName = modelIndex.data().toString();
	return tempRuleName;
}

///----get current QComboBox index of prority name-------------------
QString QGenieQosObject::getPrority(int index /* = 2 */)
{
	QString tempPriority ="";
	switch(index)
	{
	case 0:
		tempPriority = "Highest";
		break;
	case 1:
		tempPriority = "High";
		break;
	case 2:
		tempPriority = "Normal";
		break;
	case 3:
		tempPriority = "Low";
		break;
	default:
		tempPriority = "Normal";
		break;
	}
	return tempPriority;
}

QString QGenieQosObject::getConnectionType(int index)
{
	QString connType = "";
	switch(index)
	{
	case 0:
		connType = "TCP/UDP";
		break;
	case 1:
		connType = "TCP";
		break;
	case 2:
		connType = "UDP";
		break;
	default:
		connType = "TCP/UDP";
		break;
	}
	return connType;
}

///----- get mac address of user input----
QString QGenieQosObject::getMacAddress()
{
	QWidget* widge = mW->findChild<QWidget*>("macWidget");
	QList<QLineEdit*> leList = widge->findChildren<QLineEdit *>();
	int num = leList.count();
	QString macAddress = "";
	for(int i = 0;i < num; i++)
	{
		macAddress += leList[i]->text().trimmed();
	}
	return macAddress;
}