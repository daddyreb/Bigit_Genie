#include "TestTopologyForm.h"
#include "ui_TestTopologyForm.h"
#include "topology.h"
#include "SVTController.h"
#define SHOWING_ONE_PAGE 6

TestTopologyForm::TestTopologyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestTopologyForm),
//    mSVTCore(new SVT_Core),
    mController(new SVT_ControlProxy),
    mAllCount(0)
    //mCurrentPage(0),
    //mCurrentIface(-1)
{
    ui->setupUi(this);
    initHeaders();
    connect(mController,SIGNAL(signal_nodeDataReturned(SVT_NotifyType)),this,SLOT(slot_DataReturned(SVT_NotifyType)));
    connect(mController,SIGNAL(signal_countChanged(int)),this,SLOT(slot_CountChanged(int)));
    connect(mController,SIGNAL(signal_ifaceDataReturned(SVT_NotifyType)),this,SLOT(slot_IfaceReturned(SVT_NotifyType)));
    connect(mController,SIGNAL(signal_gatewayChanged(QString)),this,SLOT(slot_GatewayChanged(QString)));
    connect(mController,SIGNAL(signal_newDeviceAdded(SVT_NotifyType)),this,SLOT(slot_NewDeviceAdded(SVT_NotifyType)));
    connect(mController,SIGNAL(signal_NetgearInfo(QMap<int,QString>)),this,SLOT(slot_NetgearInfoReturned(QMap<int,QString>)) );
    mController->startControl();
}

void TestTopologyForm::initHeaders()
{
    QTableWidget *tableWidget=ui->tableWidget;
    const char * cHeaders[NIE_NUM]={
        "NIE_IP",
        "NIE_MAC",
        "NIE_LOCALNAME",
        "NIE_LOCALOSVERSION",
        "NIE_LOCALPOOLNUMBER",
        "NIE_ONLINE",
        "NIE_ISHOST",
        "NIE_ISWIRED",
        "NIE_ISGATEWAY",
        "NIE_ISDEFAULT_GATEWAY",
        "NIE_ISVIRTUALMACHINE",
        "NIE_NETBIOS_VERSION",
        "NIE_NETBIOS_HOSTNAME",
        "NIE_NETBIOS_MACVERSION",
        "NIE_NETBIOS_MACHOSTNAME",
        "NIE_UPNP_DEVICETYPE",
        "NIE_UPNP_FRIENDLYNAME",
        "NIE_UPNP_MANUFACTURER",
        "NIE_UPNP_MANUFACTURER_URL",
        "NIE_UPNP_MODELDISCRIPTION",
        "NIE_UPNP_MODELNAME",
        "NIE_CURRENTSETTING_FIRMWARE",
        "NIE_CURRENTSETTING_REGIONTAG",
        "NIE_CURRENTSETTING_REGION",
        "NIE_CURRENTSETTING_MODEL",
        "NIE_CURRENTSETTING_ROUTELOGINDOTNETIP",
        "NIE_CURRENTSETTING_RSLEVEL",
        "NIE_CURRENTSETTING_MESSAGETYPE",
        "NIE_ATTACHDEVICE_NAME",
        "NIE_ATTACHDEVICE_ISWIRED",
        "NIE_ATTACHDEVICE_LINKSPEED",
        "NIE_ATTACHDEVICE_SIGNALSTRENGTH",
        "NIE_UPDATE1_VERSION",
        "NIE_UPDATE1_NEEDUPDATE",
        "NIE_UPDATE1_ADDR",
        "NIE_UPDATE1_MD5",
        "NIE_UPDATE1_SIZE",

      //  "NIE_NUM"




    };
    tableWidget->setColumnCount(NIE_NUM);
    for(int i=0;i<NIE_NUM;i++)
    {
        QTableWidgetItem *headeritem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(i, headeritem);
        headeritem->setText(cHeaders[i]);
        headeritem->setToolTip(cHeaders[i]);
    }
}

TestTopologyForm::~TestTopologyForm()
{
    delete ui;
//    delete mSVTCore;
}

void TestTopologyForm::slot_DataReturned(SVT_NotifyType data)
{
    qDebug()<<"TestTopologyForm::slot_DataReturned";
//    QString text;
//    for(int i=0;i<data.count();i++)
//    {
//        text+=data[i].value(NIE_IP)+" "+data[i].value(NIE_ONLINE)+" ";
//        text+=data[i].value(NIE_NETBIOS_HOSTNAME)+" "+data[i].value(NIE_NETBIOS_VERSION)+" ";
//        text+=data[i].value(NIE_NETBIOS_MACHOSTNAME)+" "+data[i].value(NIE_NETBIOS_MACVERSION)+" ";
//        text+="\n";

//    }
//    ui->textBrowser->setText(text);

    QAbstractItemModel *model=ui->tableWidget->model();
    int allrows=model->rowCount();
    model->removeRows(0,allrows);
    for(int i=0;i<data.count();i++)
    {
        int row=model->rowCount();
        model->insertRows(row,1);
        QMap<int, QString>::const_iterator it = data[i].begin();
        while (it != data[i].end())
        {
            qDebug()<<it.key()<<it.value();
            model->setData(model->index(i,it.key()),it.value());
            it++;
        }
    }
//    ui->tableWidget->setColumnHidden(0,true);
}
void TestTopologyForm::slot_IfaceReturned(SVT_NotifyType data)
{
    qDebug()<<"TestTopologyForm::slot_IfaceReturned"<<data;
    ui->comboBox->clear();
    mIfaceMap.clear();
    for(int i=0;i<data.count();i++)
    {
        ui->comboBox->addItem(
                    data[i].value(IIE_NAME) +" "+data[i].value(IIE_ISON)+" "+data[i].value(IIE_ISVISUAL)
                    );
        bool btemp;
        int itemp=data[i].value(IIE_IDX).toInt(&btemp);
        if(btemp)
        {
            mIfaceMap.insert(i,itemp);
        }

    }
}

void TestTopologyForm::slot_NewDeviceAdded(SVT_NotifyType data)
{
    qDebug()<<"TestTopologyForm::slot_NewDeviceAdded";
    for(int i=0;i<data.count();i++)
    {

        qDebug()<<data[i].value(NIE_MAC) +" "+data[i].value(NIE_IP);


    }
}

void TestTopologyForm::slot_CountChanged(int c)	
{
    qDebug()<<"TestTopologyForm::slot_CountChanged";
    mAllCount=c;
    ui->lab_count->setText(QString::number(c));
    int start=ui->edt_start->text().toInt();
    int count=ui->edt_count->text().toInt();
    if(start>=mAllCount && start!=0)
    {
        subscribeCurpage();
    }
}

void TestTopologyForm::slot_GatewayChanged(QString gateway)
{
    ui->lab_gateway->setText(gateway);
}

void TestTopologyForm::slot_NetgearInfoReturned(const QMap<int, QString> &data)
{
    QString text;
    foreach(QString test,data.values())
    {
        text+=test+" ";
    }
    ui->textBrowser->setText(text);

}

void TestTopologyForm::subscribeCurpage()
{
    int start=ui->edt_start->text().toInt();
    int count=ui->edt_count->text().toInt();
    if(start>=mAllCount)
    {
        start=(0==mAllCount)?0:((mAllCount-1)/count*count);
        ui->edt_start->setText(QString::number(start));
    }


    int itemp=ui->comboBox->currentIndex();
    int ifaceidx=(itemp>=0)?mIfaceMap.value(itemp,-1):-1;
    if(ifaceidx >=0)
    {
        mController->subscrib(ifaceidx,start,count);
    }

}

void TestTopologyForm::on_but_last_clicked()
{
    int start=ui->edt_start->text().toInt();
    int count=ui->edt_count->text().toInt();
    start-=count;
    start=qMax(start,0);
    ui->edt_start->setText(QString::number(start));
    subscribeCurpage();
}

void TestTopologyForm::on_but_next_clicked()
{
    int start=ui->edt_start->text().toInt();
    int count=ui->edt_count->text().toInt();
    if(start+count<mAllCount)
    {
        start+=count;

        ui->edt_start->setText(QString::number(start));
        subscribeCurpage();
    }
}

void TestTopologyForm::on_comboBox_currentIndexChanged(int index)
{
//    if(index < 0)
//    {
//        return;
//    }
//    QComboBox *comb=qobject_cast<QComboBox *>(sender());
//    if(comb)
//    {
//        mCurrentIface=mIfaceMap.value(index,-1);
//        subscribeCurpage();
//    }
}

void TestTopologyForm::on_pushButton_clicked()
{
    subscribeCurpage();
}

void TestTopologyForm::on_checkBox_upnp_clicked(bool checked)
{
    for(int i=NIE_UPNP_DEVICETYPE;i<=NIE_UPNP_MODELNAME;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}

void TestTopologyForm::on_checkBox_currentsetting_clicked(bool checked)
{
    for(int i=NIE_CURRENTSETTING_FIRMWARE;i<=NIE_CURRENTSETTING_MESSAGETYPE;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}



void TestTopologyForm::on_checkBox_attachdevice_clicked(bool checked)
{
    for(int i=NIE_ATTACHDEVICE_NAME;i<=NIE_ATTACHDEVICE_SIGNALSTRENGTH;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}

void TestTopologyForm::on_checkBox_netbios_clicked(bool checked)
{
    for(int i=NIE_NETBIOS_VERSION;i<=NIE_NETBIOS_MACHOSTNAME;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}

void TestTopologyForm::on_checkBox_flags_clicked(bool checked)
{
    for(int i=NIE_ONLINE;i<=NIE_ISVIRTUALMACHINE;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}

void TestTopologyForm::on_checkBox_all_clicked(bool checked)
{
    on_checkBox_upnp_clicked(checked);
    on_checkBox_currentsetting_clicked(checked);
    on_checkBox_attachdevice_clicked(checked);
    on_checkBox_netbios_clicked(checked);
    on_checkBox_flags_clicked(checked);
    on_checkBox_local_clicked(checked);
    on_checkBox_fupdate_clicked(checked);
}

void TestTopologyForm::on_checkBox_local_clicked(bool checked)
{
    for(int i=NIE_LOCALNAME;i<=NIE_LOCALPOOLNUMBER;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}

void TestTopologyForm::on_pushButton_refresh_clicked()
{
    mController->refresh();
}

void TestTopologyForm::on_but_netgearinfo_clicked()
{
    mController->refreshNetgearInfo();
}

void TestTopologyForm::on_checkBox_fupdate_clicked(bool checked)
{
    for(int i=NIE_UPDATE1_VERSION;i<=NIE_UPDATE1_SIZE;i++)
    {
        ui->tableWidget->setColumnHidden(i,!checked);
    }
    if(!checked)
    {
        ui->checkBox_all->setChecked(false);
    }
}
