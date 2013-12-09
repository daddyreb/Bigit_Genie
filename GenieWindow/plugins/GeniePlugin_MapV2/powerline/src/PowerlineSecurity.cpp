#include "PowerlineSecurity.h"
#include "ui_PowerlineSecurity.h"
#include "Powerline_mapuitilities.h"
#include "PowerlinePrompt.h"
#include "Powerline_MessageBox.h"
#include "DialogMgr.h"
#include "PowerlineMapView.h"

#include <QCheckBox>
#include <QMessageBox>





PowerlineSecurity::PowerlineSecurity(QWidget *parent) :
QDialog(parent),
ui(new Ui::PowerlineSecurity)
,m_bMove(false)
{
	ui->setupUi(this);

	setHeaderStyle();
	slotConnect();
	clearData();
	initLanguage();
	retranslateUi();
}

PowerlineSecurity::~PowerlineSecurity()
{
	ui->table_secu->clearContents();
	delete ui;

	qDebug() << "PowerlineSecurity exit";
}


//====================================================
//  Initialization function
//====================================================
void PowerlineSecurity::initLanguage()
{
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_SEC_TITLE, "Security");
	POWERLINE_TRANSLATE_RADIO(ui->rdo_new, L_POWERLINE_SEC_RADIO_NEW, "Create a  new Entyption Key");
	POWERLINE_TRANSLATE_RADIO(ui->rdo_default, L_POWERLINE_SEC_RADIO_DEF, "Use default Entyption Key");
	POWERLINE_TRANSLATE_BUTTON(ui->but_apply, L_POWERLINE_SEC_BUT_APPLY, "Apply Security to Selected Devices");
	POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CLOSE, "Close");
}
void PowerlineSecurity::setHeaderStyle()
{
	const QString tmpHeaderStyle = (
		"QHeaderView::section {"
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb(240, 240,251), stop:1 rgb( 116, 113, 190));"
		"color:rgb(255,255,255);"
		"border-color:rgb( 255, 255, 255);"
		"border-style:solid;"
		"border-left-width:1px;height:25px;"
		"border-right-width:1px;"
		"}");
	const QString tmpStyle = (
		"QTableWidget{"
		"border-style:solid;border-width:1px;"
		"border-color:rgb(203, 197, 235);"
		"}");
	ui->table_secu->horizontalHeader()->setStyleSheet(tmpHeaderStyle);
	ui->table_secu->setStyleSheet(tmpStyle);

	ui->table_secu->verticalHeader()->setVisible(false);
	ui->table_secu->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui->table_secu->setEditTriggers(QAbstractItemView::NoEditTriggers);

	ui->table_secu->setSelectionMode(QAbstractItemView::SingleSelection);


}
void PowerlineSecurity::slotConnect()
{
	connect(ui->but_close, SIGNAL(clicked()), this, SLOT(slot_BtnClose()));
	connect(ui->but_refresh, SIGNAL(clicked()), this, SLOT(slot_BtnRefresh()));
	connect(ui->but_apply, SIGNAL(clicked()), this, SLOT(slot_BtnApply()));


	connect(ui->table_secu, SIGNAL(cellChanged(int,int)), this, SLOT(slot_chkState(int, int)));

	connect(&m_pwdDlg, SIGNAL(signBtnOk(const QList<QPair<QString, QString> >&)), this, SLOT(slot_retPwdDlg(const QList<QPair<QString, QString> >&)));
	//connect(ui->lab_refresh, SIGNAL(valueChanged(int)), this, SLOT(slot_Progress(int)));
	connect(ui->rdo_default, SIGNAL(clicked()), this, SLOT(slot_rdo_default()));
	connect(ui->rdo_new, SIGNAL(clicked()), this, SLOT(slot_rdo_new()));
}

void PowerlineSecurity::clearData()
{
	ui->table_secu->clearContents();
	ui->edt_key->clear();
	ui->rdo_default->setChecked(true);
	ui->edt_key->setMaxLength(64);
	ui->edt_key->setValidator(new QRegExpValidator(QRegExp("([A-Z]|[a-z]|[0-9]){64}"), this));

	/////
	ui->but_apply->setDisabled(true);
	ui->but_refresh->setDisabled(true);

	ui->lab_refresh->setHidden(true);
	ui->but_refresh->setHidden(true);

	ui->but_close->setFocus();

	ui->lab_localinfo->hide();

	m_strMac.clear();
	m_model.clear();

	slot_rdo_default();

}
void PowerlineSecurity::initializeValue(QList<PLDevice *> devList)
{
	int nDevNum			= devList.count();
	QString strDevName;
	QString strDevModel;
	QString strDevMacAddr;
	QString strDevPwd;
	QStringList strList;


	PowerlineMapView *pView = PowerlineMapView::GetSingletonPtr();
	Q_ASSERT(pView);


	clearData();

	ui->table_secu->setRowCount(nDevNum);
	m_devInfo.clear();
	m_chk_devInfo.clear();
	int nIdx = 0;
	//int nDx = 0;
	bool isLocalOk = false;
	QStringList strLst;


	for(int i = 0; i < nDevNum; ++i)
	{
		strDevName		= (*(devList[i]))[GDP_NAME].toString();
		strDevModel		= (*(devList[i]))[GDP_MODEL_NAME].toString();
		strDevMacAddr	= (*(devList[i]))[GDP_PHYSICALADDR].toString();

		strDevPwd.clear();

		if(!strDevMacAddr.isEmpty())
		{
			pView->loadPLCDevicePassword(strDevMacAddr, &strDevPwd);
		}

		if(strDevName.isEmpty())
		{
			strDevName = "un-named";
		}
		strList.clear();
		if(pView->isLocalDev(strDevMacAddr))
		{
			//Local Dev		
			isLocalOk = true;
			
			strDevPwd = "XXXX-XXXX-XXXX-XXXX";

			m_strMac = strDevMacAddr;
			m_model = strDevModel;
			m_strMac.replace("-", ":");
			QString strLocalDev = POWERLINE_GET_TEXT(L_POWERLINE_SEC_TIP, "Password is not required for local device(%1, MAC %2)");
#ifdef TEST_PL_CODE
			strLocalDev = strLocalDev.arg(m_model).arg(m_strMac);
#else
			strLocalDev += "(%1, MAC %2)";
			strLocalDev = strLocalDev.arg(m_model).arg(m_strMac);
#endif
			ui->lab_localinfo->setText(strLocalDev);
			ui->lab_localinfo->show();
			strList << strDevName << strDevMacAddr << strDevPwd << QString("%1").arg(0);
			m_devInfo.insert(0, strList);

			strLst.insert(0, strDevModel);
		}
		else
		{
			strList << strDevName << strDevMacAddr << strDevPwd << QString("%1").arg(nIdx + 1);
			m_devInfo.append(strList);

			strLst.append(strDevModel);
		}
	
		nIdx++;
	}


		//        ui->table_secu->setCellWidget(i, 0, new QCheckBox(strDevName, this));

	for(int i = 0; i < nDevNum; ++i)
	{
		QTableWidgetItem *itemChkBox    = new QTableWidgetItem();

		QTableWidgetItem *itemModel		= new QTableWidgetItem();
		QTableWidgetItem *itemMacAddr   = new QTableWidgetItem();
		QTableWidgetItem *itemPwd		= new QTableWidgetItem();
		//m_devInfo.append(strList);

		//strLst.append(strDevModel);

		strDevName		= m_devInfo[i].at(0);
		strDevModel		= strLst[i];
		strDevMacAddr	= m_devInfo[i].at(1);
		strDevPwd		= m_devInfo[i].at(2);

		itemModel->setText(strDevModel);
		itemMacAddr->setText(strDevMacAddr);
		itemPwd->setText(strDevPwd);

		itemChkBox->setCheckState(Qt::Checked);
		itemChkBox->setText(strDevName);

		ui->table_secu->setItem(i, 0, itemChkBox);
		ui->table_secu->setItem(i, 1, itemModel);
		ui->table_secu->setItem(i, 2, itemMacAddr);
		ui->table_secu->setItem(i, 3, itemPwd);

		ui->table_secu->item(i, 1)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->table_secu->item(i, 2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui->table_secu->item(i, 3)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	}

	m_chk_devInfo = m_devInfo;
	qDebug() << "initValue: " << m_chk_devInfo;

	ui->but_apply->setDisabled(false);
}
//====================================================
//  Slot function
//====================================================

void PowerlineSecurity::slot_rdo_default()
{
	ui->edt_key->setDisabled(true);
}

void PowerlineSecurity::slot_rdo_new()
{
	ui->edt_key->setDisabled(false);
	ui->edt_key->setFocus();
}
void PowerlineSecurity::slot_BtnClose()
{

	this->reject();
}

void PowerlineSecurity::slot_BtnRefresh()
{
	ui->lab_refresh->setValue(0);
	ui->lab_refresh->setRange(0, 40);

	for(int i = 0; i < 100; i++)
	{

		ui->lab_refresh->setValue(i);
	}
}
void PowerlineSecurity::slot_BtnApply()
{
	int nRet = 0;
	bool bRet       = false;
	DialogMgr *dlgMgr = DialogMgr::GetSingletonPtr();
	Q_ASSERT(dlgMgr);

	nRet = rdoCheck();
	if(nRet == RS_DEFAULT)
	{
		m_key = DEFAULT_KEY;
	}
	else
	{
		bRet = keyCheck();
		if(!bRet)
		{
			//Powerline_MessageBox box;
			//box.showDlg("Warning", "The Encryption Key Entered is Invalid.Please Enter Encryption Key between 8 and 64 characters.");
			//box.exec();
			QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_MSG_TITLE_F, "Warning");
			QString strExplain = POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG1_EXPLAIN, "The Encryption Key Entered is Invalid.Please Enter Encryption Key between 8 and 64 characters.");
			dlgMgr->showMsgBox(strTitle, strExplain);


			ui->edt_key->clear();
			return;
		}

		m_key = ui->edt_key->text();
	}



	QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_SEC_TITLE, "Security");
	QString strContent = POWERLINE_GET_TEXT(L_POWERLINE_LED_EXPLAIN, "Connectivity will be temporarily lost if  you continue with this setting Click 'OK' to proceed");

	nRet = dlgMgr->showMsgBox(strTitle, strContent, PL_MB_OK | PL_MB_CANCEL);
	if(nRet != 1)
	{
		return;
	}

	////////如果要修改的Item已从配置文件中读出了Password就不需要弹出下级dlg
	bRet = chkAndDeal(m_chk_devInfo);
	if(bRet && m_lstData.count() > 0)
	{
		qDebug() << "Submit Data: " << m_lstData;
		slot_retPwdDlg(m_lstData);
		return;
	}

	////////end

	qDebug() << "m_chk_devInfo:" << m_chk_devInfo;
	wipeOutRepeatItem(m_chk_devInfo);


	m_pwdDlg.initializedTable(m_chk_devInfo);
	m_pwdDlg.showDlg();

}
void PowerlineSecurity::slot_chkState(int row, int col)
{
	if(ui->table_secu->item(row, col)->checkState() == Qt::Checked)
	{
		qDebug() << "slot_chkState m_chkDevInfo:" << m_devInfo[row];

		if(row == 0)
		{
			m_chk_devInfo.insert(row, m_devInfo[row]);
		}
		else
		{
			m_chk_devInfo.append(m_devInfo[row]);
		}
		
		ui->but_apply->setDisabled(false);
	}
	else if(row < m_devInfo.count())
	{
		for(int i = 0; i < m_chk_devInfo.count(); ++i)
		{
			if(m_devInfo[row].at(3).toInt() == m_chk_devInfo[i].at(3).toInt())
			{
				m_chk_devInfo.removeAt(i);
				break;
			}
		}
	}

	if(m_chk_devInfo.count() == 0)
	{
		ui->but_apply->setDisabled(true);
	}

	qDebug() << "slot_chkState: " << m_chk_devInfo;
}
void PowerlineSecurity::slot_retPwdDlg(const QList<QPair<QString, QString> > &lst)
{
	PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
	Q_ASSERT(pMapView);

	pMapView->initControlBtn();
	emit signApply(m_key, lst);
	this->accept();
}
void PowerlineSecurity::slot_Progress(int num)
{
	Q_UNUSED(num)
}
//====================================================
//  other function
//====================================================
bool PowerlineSecurity::chkAndDeal(const QList<QStringList> &data)
{
	qDebug() << "PowerlineSecurity::chkAndDeal();";

	int nCount = 0;

	nCount	= data.count();
	QString strDevMacAddr;
	QString strDevPwd;
	for(int i = 0; i < nCount; ++i)
	{
		if(data[i].count() != 4)
		{
			m_lstData.clear();
			return false;
		}

		if(data[i].at(2).isEmpty())
		{
			m_lstData.clear();
			return false;
		}
		
		strDevMacAddr	= data[i].at(1);
		strDevPwd		= data[i].at(2);
		m_lstData.append(QPair<QString, QString>(strDevMacAddr, strDevPwd));
	}


	return true;
}
void PowerlineSecurity::retranslateUi()
{
	QString strStep1, strStep2, strStep3, strStep;
	QString strLocalDev;
	QString strLstName, strLstModel, strLstMac, strLstPwd;

	strStep		= POWERLINE_GET_TEXT(L_POWERLINE_SEC_STEP, "Step");
	strStep1	= QString("%1%2 %3").arg(strStep).arg(1).arg(POWERLINE_GET_TEXT(L_POWERLINE_SEC_STEP1, "Select Devices"));
	strStep2	= QString("%1%2 %3").arg(strStep).arg(2).arg(POWERLINE_GET_TEXT(L_POWERLINE_SEC_STEP2, "Select a Encryption key"));
	strStep3	= QString("%1%2 %3").arg(strStep).arg(3).arg(POWERLINE_GET_TEXT(L_POWERLINE_SEC_STEP3, "Apply Changes"));

	strLocalDev = POWERLINE_GET_TEXT(L_POWERLINE_SEC_TIP, "Password is not required for local device(%1, MAC %2)");
	strLocalDev.arg(m_model, m_strMac);

	strLstName	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_NAME, "Device Name");
	strLstModel	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_MODEL, "Model");
	strLstMac	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_MAC, "MAC Address");
	strLstPwd	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_DEVPWD, "Device Password");

	ui->lab_step1->setText(strStep1);
	ui->lab_step2->setText(strStep2);
	ui->lab_step3->setText(strStep3);

	ui->lab_localinfo->setText(strLocalDev);

	QTableWidgetItem *tabItem = ui->table_secu->horizontalHeaderItem(0);
	tabItem->setText(strLstName);
	tabItem = ui->table_secu->horizontalHeaderItem(1);
	tabItem->setText(strLstModel);
	tabItem = ui->table_secu->horizontalHeaderItem(2);
	tabItem->setText(strLstMac);
	tabItem = ui->table_secu->horizontalHeaderItem(3);
	tabItem->setText(strLstPwd);
}
PowerlineSecurity::RADIO_SEL PowerlineSecurity::rdoCheck()
{
	if(ui->rdo_default->isChecked())
	{
		return RS_DEFAULT;
	}

	return RS_NEW;
}

bool PowerlineSecurity::keyCheck()
{
	QString strTmp = ui->edt_key->text();
	int     nLen   = strTmp.length();

	if(nLen < 8 || nLen > 64)
	{
		return false;
	}

	return true;
}
void PowerlineSecurity::wipeOutRepeatItem(QList<QStringList> &data)
{
	/////去除重复项

	/////test
#ifndef TEST_PL_CODE
	QList<int> nLst;
	for(int i = 0; i < ui->table_secu->rowCount(); ++i)
	{
		if(ui->table_secu->item(i, 0)->checkState() == Qt::Checked)
		{
			nLst.append(i);
		}
	}
	qDebug() << "wipeOutRepeatItem: " << nLst;
#endif
	/////end

	for(int i = 0; i < data.count() - 1; ++i)
	{
		for(int j = i + 1; j < data.count(); ++j)
		{
			if(data[i].at(3).toInt() == data[j].at(3).toInt())
			{
				data.removeAt(j);
				j--;
			}
		}
	}
}
//====================================================
//  event function
//====================================================
void PowerlineSecurity::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void PowerlineSecurity::mousePressEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}
}
void PowerlineSecurity::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}
}
void PowerlineSecurity::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}