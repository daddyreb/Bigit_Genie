#include "PowerlineSecurity_PWD.h"
#include "ui_PowerlineSecurity_PWD.h"
#include "Powerline_mapuitilities.h"
#include "PowerlineDefine.h"
#include "PowerlineMapView.h"

#include <QMouseEvent>
#include <QRegExp>
#include <QDebug>
#include <QLineEdit>
#include <QEvent>
#include "language_Map.h"




PowerlineSecurity_PWD::PowerlineSecurity_PWD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerlineSecurity_PWD)
  ,m_bMove(false)
  ,m_dealItemNum(0)
  ,m_Dealed(0)
  ,m_bOk(false)
{
	ui->setupUi(this);

	setHeaderStyle();
	slotConnect();
	setPic(DEFAULT_PIC_PATH);
	init();
	initLanguage();
	retranslateUi();
}

PowerlineSecurity_PWD::~PowerlineSecurity_PWD()
{
    ui->table_secu->clear();
    delete ui;

	qDebug() << "PowerlineSecurity_PWD exit";
}
//====================================================
//  Initialization function
//====================================================
void PowerlineSecurity_PWD::initLanguage()
{
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_SEC_DLG2_TITLE, "Device Password");
	POWERLINE_TRANSLATE_LABEL(ui->lab_explain1, L_POWERLINE_SEC_DLG2_EXPLAIN1, "The Device Password typically appears as a number and letter code,in groups of four.separated by dashes.");
	POWERLINE_TRANSLATE_LABEL(ui->lab_explain2, L_POWERLINE_SEC_DLG2_EXPLAIN2, "[for example:FBWP-QLQL-VNRE-HQUD].It is printed on the back of each device under PWD.");
	POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CANCEL, "Cancel");
	POWERLINE_TRANSLATE_BUTTON(ui->but_ok, L_POWERLINE_BUT_APPLY, "Apply");
	
}
void PowerlineSecurity_PWD::init()
{
	ui->table_secu->clearContents();
	m_lstData.clear();
	m_dealItemNum = 0;
	m_Dealed = 0;

	//ui->table_secu->installEventFilter(this);

	ui->but_close->setFocus();
	///////////
	ui->but_ok->setDisabled(true);
}
void PowerlineSecurity_PWD::setHeaderStyle()
{
    const QString tmpHeaderStyle = (
                "QHeaderView::section {"
                "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb(  240, 240,251), stop:1 rgb( 116, 113, 190));"
                "color:rgb(255,255,255);"
                "border-color:rgb( 255, 255, 255);"
                "border-style:solid;"
                "border-left-width:1px;height:25px;"
                "border-right-width:1px;"
                "}"
                /*		"QHeaderView::section:checked{background-color: red;}"
                                                                                                  "QHeaderView::down-arrow {image: url(down_arrow.png);}"
                                                                                                  "QHeaderView::up-arrow {image: url(up_arrow.png);}"*/);
    const QString tmpStyle = (
                "QTableWidget{"
                "border-style:solid;border-width:1px;"
                "border-color:rgb(203, 197, 235);"
				"color:rgb(255, 0, 0);"
                "}");
    ui->table_secu->horizontalHeader()->setStyleSheet(tmpHeaderStyle);
    ui->table_secu->setStyleSheet(tmpStyle);

    ui->table_secu->verticalHeader()->setVisible(false);
    //ui->table_secu->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->table_secu->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_secu->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void PowerlineSecurity_PWD::slotConnect()
{
    connect(ui->but_close, SIGNAL(clicked()), this, SLOT(slot_close()));
    connect(ui->but_ok, SIGNAL(clicked()), this, SLOT(slot_ok()));

    connect(ui->table_secu, SIGNAL(cellChanged(int,int)), this, SLOT(slot_dealCellChg(int, int)));

}
void PowerlineSecurity_PWD::setPic(const QString &str)
{
    QPixmap tPix;
    tPix.load(str);
    ui->lab_pic->setPixmap(tPix);
}
//====================================================
//  slot function
//====================================================
void PowerlineSecurity_PWD::slot_close()
{
	this->reject();
}
void PowerlineSecurity_PWD::OnChanged(const QString& text)
{
	Q_UNUSED(text)
	QLineEdit * target = NULL;

	QRegExp regExp("([A-Z]){4}(-([A-Z]){4}){3}");
	bool b = false;
	for(int i= 0; i <ui->table_secu->rowCount(); ++i)
	{
		target = qobject_cast<QLineEdit *> (ui->table_secu->cellWidget(i, 2));
		qDebug() << target->text();
		if(!regExp.exactMatch(target->text()))
		{
			ui->but_ok->setDisabled(true);
			b = true;
			return;
		}
	}

	if(!b)
	{
		ui->but_ok->setDisabled(false);
	}

}
void PowerlineSecurity_PWD::OnEdited(const QString& text)
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());

	if(0 == lineEdit){
		return;
	}

	int oldLen = lineEdit->property(OLD_TEXT_LEN).toInt();

	QString strNewText = text.toUpper();

	if(strNewText.length() == 4 || strNewText.length() == 9 ||
		strNewText.length() == 14){

			if(oldLen < strNewText.length()){
				strNewText += "-";

			}else{
				strNewText = strNewText.mid(0, strNewText.length() - 1);
			}
	}

	lineEdit->setText(strNewText);
	lineEdit->setProperty(OLD_TEXT_LEN, lineEdit->text().length());
}
void PowerlineSecurity_PWD::slot_ok()
{
    //if(m_lstData.count() != (ui->table_secu->rowCount())
    //{
    //    return;
    //}

	QLineEdit *target = NULL;
	QString strMac;
	QString strTxt;

	//if(!m_bOk)
	//{
		m_lstData.clear();

		for(int i = 0; i < ui->table_secu->rowCount(); ++i)
		{
			target = qobject_cast<QLineEdit *> (ui->table_secu->cellWidget(i, 2));
			strMac = ui->table_secu->item(i, 1)->text();
			strTxt = target->text();

			m_lstData.append(QPair<QString, QString>(strMac, strTxt));
		}
	//}


	
	

	wipeOutRepeatItem(m_lstData);
qDebug() << m_lstData;

    emit signBtnOk(m_lstData);
    this->accept();
}
void PowerlineSecurity_PWD::slot_dealCellChg(int row, int col)
{
    if(col < 2)
    {
        return;
    }
    QString strTxt = ui->table_secu->item(row, 2)->text();
    bool bRet = valueChk(strTxt);
    if(!bRet)
    {
		//if(!m_lstData[row].second.isEmpty())
		//{
		//	ui->table_secu->item(row, col)->setText(m_lstData[row].second);
		//}
		//else
		//{
			ui->table_secu->item(row, col)->setText("<Click here to enter the password>");
			ui->but_ok->setDisabled(true);

		//}
        return;
    }

    QString strMac = ui->table_secu->item(row, 1)->text();

    for(int i = 0; i < m_lstData.count(); ++i)
    {
        if(m_lstData[i].first == strMac)
        {
            m_lstData[i].second = strTxt;
			if(m_lstData.count() == ui->table_secu->rowCount())
			{
				ui->but_ok->setDisabled(false);
			}
            return;
        }
    }
    m_lstData.append(QPair<QString, QString>(strMac, strTxt));

    if(m_lstData.count() == ui->table_secu->rowCount())
    {
        ui->but_ok->setDisabled(false);
    }

}
//====================================================
//  other function
//====================================================
void PowerlineSecurity_PWD::retranslateUi()
{
	QString strLstName, strLstMac, strLstPwd;
	QString strGroupTitle;
	QString strLstEdit;

	strLstName	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_NAME, "Device Name");
	strLstMac	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_MAC, "MAC Address");
	strLstPwd	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_LIST_DEVPWD, "Device Password");

	strGroupTitle = POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG2_GROUP_TITLE, "Please enter a Password");

	strLstEdit	= POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG2_LIST_TIP, "<Click here to enter the password>");

	QLineEdit *target = NULL;

	ui->groupBox->setTitle(strGroupTitle);

	for(int i = 0; i < ui->table_secu->rowCount(); ++i)
	{
		target = qobject_cast<QLineEdit *> (ui->table_secu->cellWidget(i, 2));
		target->setText(strLstEdit);
	}

	QTableWidgetItem *tabItem = ui->table_secu->horizontalHeaderItem(0);
	tabItem->setText(strLstName);
	tabItem = ui->table_secu->horizontalHeaderItem(1);
	tabItem->setText(strLstMac);
	tabItem = ui->table_secu->horizontalHeaderItem(2);
	tabItem->setText(strLstPwd);


}
bool PowerlineSecurity_PWD::valueChk(const QString &str)
{
    QString strTmp = str.toUpper();
    /*QRegExp regExp("^([A-F]{4})(-[A-F]{4}){3}$");*/
    QRegExp regExp("^((([A-Z]|[0-9]){4}-){3})(([A-Z]|[0-9]){4})$");

    return regExp.exactMatch(strTmp);
}
void PowerlineSecurity_PWD::wipeOutRepeatItem(QList<QPair<QString, QString> > &data)
{
	/////È¥³ýÖØ¸´Ïî
	if(data.count() < 1)
	{
		return;
	}

	//if(data[0].first == m_strLocalDevice)
	//{
	//	data.removeAt(0);
	//}

	for(int i = 0; i < data.count() - 1; ++i)
	{
		for(int j = i + 1; j < data.count(); ++j)
		{
			if(data[i].first == data[j].first)
			{
				data.removeAt(j);
			}
		}
	}
}
//====================================================
//  Public function
//====================================================
void PowerlineSecurity_PWD::showDlg()
{
    QWidget *w;

    w = static_cast<QWidget *>(this);
    setFramelessStyle(w);


    w->show();
}

void PowerlineSecurity_PWD::initializedTable(QList<QStringList> strList)
{
    int nDevNum			= strList.count();
    QString strDevName;
    QString strDevMacAddr;
    QString strDevPwd;
	int		nIdx;
    bool bEdit = false;

	QString strTmpTip = POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG2_LIST_TIP, "<Click here to enter the password>");


    init();

	qDebug() << "strList: " <<strList;
    ui->table_secu->setRowCount(nDevNum);
	m_dealItemNum = nDevNum;
	bool bF =false;
	
    for(int i = 0; i < nDevNum; ++i)
    {


        if(strList[i].count() != 4)
        {
			bF = true;
            break;
        }
        strDevName		= strList[i].at(0);
        strDevMacAddr	= strList[i].at(1);
        strDevPwd		= strList[i].at(2);
		nIdx			= strList[i].at(3).toInt();

        if(strDevName.isEmpty())
        {
            strDevName = "un-named";
        }

		bEdit = true;
		if(nIdx == 0)
		{
			/////LocalDevice
			strDevPwd = "XXXX-XXXX-XXXX-XXXX";
			m_strLocalDevice = strDevMacAddr;
			bEdit = false;
			m_dealItemNum -= 1;
		}
        else if(strDevPwd.isEmpty())
        {
            strDevPwd = strTmpTip;
            
        }
		else
		{
			m_lstData.append(QPair<QString, QString>(strDevMacAddr, strDevPwd));
		}


        QTableWidgetItem *itemName	= new QTableWidgetItem();
        QTableWidgetItem *itemMacAddr	= new QTableWidgetItem();
        //QTableWidgetItem *itemPwd	= new QTableWidgetItem();

        itemName->setText(strDevName);
        itemMacAddr->setText(strDevMacAddr);
        //itemPwd->setText(strDevPwd);

		QLineEdit *m_tableEditer = new QLineEdit;
		m_tableEditer->installEventFilter(this);
		m_tableEditer->setText(strDevPwd);
		m_tableEditer->setFrame(false);
		m_tableEditer->setProperty(OLD_TEXT_LEN, m_tableEditer->text().length());
        m_tableEditer->setValidator(new QRegExpValidator(QRegExp("([A-Z]|[a-z]){4}(-([A-Z]|[a-z]){4}){3}"),m_tableEditer));
		//m_tableEditer->setStyleSheet("color:rgb(255, 0, 0);");
		connect(m_tableEditer, SIGNAL(textEdited(QString)), this, SLOT(OnEdited(QString)));
		connect(m_tableEditer, SIGNAL(textChanged(QString)), this, SLOT(OnChanged(QString)));



		m_tableEditer->setEnabled(bEdit);
		ui->table_secu->setCellWidget(i,2, m_tableEditer);

        ui->table_secu->setItem(i, 0, itemName);
        ui->table_secu->setItem(i, 1, itemMacAddr);
        //ui->table_secu->setItem(i, 2, itemPwd);

        ui->table_secu->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->table_secu->item(i, 1)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);


  //      if(bEdit)
  //      {
  //          ui->table_secu->item(i, 2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
  //          bEdit = false;
  //      }
		//if(!bEdit)
		//{
		//	ui->table_secu->item(i, 2)->setFlags(Qt::ItemIsSelectable);
		//}
		if(i == 0)
		{
			QLineEdit *target = qobject_cast<QLineEdit *> (ui->table_secu->cellWidget(i, 2));
			QString strTxt = target->text();
			if(strTxt == "XXXX-XXXX-XXXX-XXXX")
			{
				QTableWidgetItem *pItem = NULL;

				pItem = ui->table_secu->item(i, 1);
				pItem->setTextColor(QColor(Qt::black));
				pItem = ui->table_secu->item(i, 0);
				pItem->setTextColor(QColor(Qt::black));
			}
			else
			{
				m_tableEditer->setStyleSheet("color:rgb(255, 0, 0);");
			}
			
		}
		else
		{
			m_tableEditer->setStyleSheet("color:rgb(255, 0, 0);");
		}

    }

	if(!bF && m_lstData.count() == m_dealItemNum)
	{
		m_lstData.insert(0, QPair<QString, QString>(strList[0].at(1), "XXXX-XXXX-XXXX-XXXX"));

		ui->but_ok->setDisabled(false);
		m_bOk = true;
	}
}
//====================================================
//  event function
//====================================================
void PowerlineSecurity_PWD::mouseMoveEvent(QMouseEvent *evt)
{
    if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
    {
        move(evt->globalPos() - m_position);
        evt->accept();
    }
}
void PowerlineSecurity_PWD::mousePressEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::LeftButton)
    {
        m_bMove = true;
        m_position = evt->globalPos() - frameGeometry().topLeft();
    }
}
void PowerlineSecurity_PWD::mouseReleaseEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::LeftButton)
    {
        m_bMove = false;
    }
}

bool PowerlineSecurity_PWD::eventFilter ( QObject * watched, QEvent * event )
{
	QLineEdit *target = qobject_cast<QLineEdit *> (watched);

	int rowCount = ui->table_secu->rowCount();
	int idx = 0;
	for (idx = 0 ; idx < rowCount ; ++idx)
	{	
		QLineEdit *tmp = qobject_cast<QLineEdit*> (ui->table_secu->cellWidget(idx, 2)) ;
		if((tmp == target) && (tmp != 0)){
			break;
		}
	}

	if (idx >= rowCount)
	{
		return false;
	}

	QString strTmpTip = POWERLINE_GET_TEXT(L_POWERLINE_SEC_DLG2_LIST_TIP, "<Click here to enter the password>");
	QString strTxt;

	if (event->type() == QEvent::FocusIn)
	{
		target->clear();
		target->setProperty(OLD_TEXT_LEN, target->text().length());

	}else if(event->type() == QEvent::FocusOut){
		strTxt = target->text();
		bool bRet = valueChk(strTxt);
		if(!bRet)
		{
			target->setText(strTmpTip);
			ui->but_ok->setDisabled(true);
	
			m_Dealed--;
			m_Dealed = (m_Dealed >= 0) ? m_Dealed : 0; 
		}
		else
		{
			m_Dealed++;
			m_Dealed = (m_Dealed <= m_dealItemNum) ? m_Dealed : m_dealItemNum;
		}
	}

	/*if(m_dealItemNum > 2)
	{
		QString strMac = ui->table_secu->item(idx, 1)->text();

		for(int i = 0; i < m_lstData.count(); ++i)
		{
			if(m_lstData[i].first == strMac)
			{
				m_lstData[i].second = strTxt;
				if(m_lstData.count() == ui->table_secu->rowCount())
				{
					ui->but_ok->setDisabled(false);
				}
				return false;
			}
		}
		m_lstData.append(QPair<QString, QString>(strMac, strTxt));

		if(m_lstData.count() == ui->table_secu->rowCount())
		{
			ui->but_ok->setDisabled(false);
		}
	}*/


	
	//if(m_Dealed == m_dealItemNum)
	//{
	//	ui->but_ok->setDisabled(false);
	//}

	return false;
}

void PowerlineSecurity_PWD::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}
