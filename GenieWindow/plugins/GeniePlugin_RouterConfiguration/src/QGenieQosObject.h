#ifndef QGENIEQOSOBJECT_H
#define QGENIEQOSOBJECT_H
#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
class QGenieQosObject : public QObject,public Singleton<QGenieQosObject>
{
	Q_OBJECT
public:
    explicit QGenieQosObject(QObject *parent = 0);
	~QGenieQosObject();
	QWidget *getQosUi();
	void retranslateUi();
	//////////////////////////////////////////////////////////////////////////
	enum COMBOXTYPE_ENUM
	{
		PRIORITY_CATEGORY
		,PRIORITY
		,APPLICATIONTYPE
		,CONNECTIONTYPE
	};
	COMBOXTYPE_ENUM mComboxType;
protected:
	QWidget *mW;
	QStackedWidget* stackMain_Qos;
	QStackedWidget* stackSecond_Qos;
	QStackedWidget* stackThird_Qos;
	QCheckBox* chk_QosSwitch;

	QLineEdit* lineEdit_QoSPolicy;
	QLineEdit* lineEdit_StartPort;
	QLineEdit* lineEdit_EndPort;
	QLineEdit* lineEdit_DeviceName;

	QComboBox* cbx_PriorityCategory;
	QComboBox* cbx_Priority;
	QComboBox* cbx_ConnectionType;
	QComboBox* cbx_Applictaions;

	QLabel* lab_Applictions;

	QTableView* tableView_RuleList;

	QMap<QString,QStringList> m_ruleList; ///用于保存规则列表
	QStringList m_AplicationItems;///保存路由器已存的应用程序
	QStringList m_OnlineGameItems;

	int mFlag; ///0:add,1:update;
public slots:
    void slot_buttonClicked();
	void slot_ckbStateChanged(int state);
	void slot_combxIndexChanged(int index);
	void slot_DataReturned(int session,const QStringList &lst);
	void slot_GetRulesDataReturned(int session,const QStringList &lst);
private:
	void initComboxItem(COMBOXTYPE_ENUM comboxType);
	void refleshRulesList();
	QString getSelectRowData();
	QString getPrority(int index = 2);
	QString getConnectionType(int index = 0);
	QString getMacAddress();
};

#endif // QGENIEQOSOBJECT_H
