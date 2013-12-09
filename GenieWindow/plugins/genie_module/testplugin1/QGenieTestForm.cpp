#include "QGenieTestForm.h"
#include "ui_QGenieTestForm.h"
#include "genie2_interface.h"
#include "genie2_routeranim.h"
#include "genie2_netgear.h"

QVariant string2variant(const QString &s)
{
    QStringList lst=s.split(",");
    if(lst.count()<=1)
    {
        bool btemp;
        int itemp=s.toInt(&btemp);
        if(btemp)
        {
            return itemp;
        }
        else
        {
            return s;
        }
    }
    int c=lst.count();
    QVariantList vlist;
    for(int i=0;i<c;i++)
    {
        vlist<<QVariant(string2variant(lst[i]));
    }
    return vlist;
}

QGenieTestForm::QGenieTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieTestForm)
{ 
//		int *itemp=new int;
    ui->setupUi(this);
    GENIE2_TRANSLATE_BUTTON(ui->tr_but,1906);
    GENIE2_TRANSLATE_LABEL(ui->tr_lab,1905);
    const char *cCombItems[]={
        "UICMD_BIGBUTTON_BUTLINE_CLICKHERE"
		,"UICMD_BIGBUTTION_BUTLINE_ERROR"
		,"UICMD_BIGBUTTION_BUTLINE_GOOD"
		,"UICMD_BIGBUTTION_BUTLINE_OVERFLOW"
		,"UICMD_BIGBUTTION_BUTLINE_FIXIT"
		,"UICMD_BIGBUTTON_ICON"
		,"UICMD_BIGBUTTON_LANGUAGE"
		,"UICMD_LEFTBUTTON_LANGUAGE"
		,"UICMD_SHOW_MAINFRAME"
		,"UICMD_SET_FRAMELESS_STYLE"
		,"UICMD_TRAY_ICO"
		,"UICMD_TRAY_MESSAGE"
		,"UICMD_TRAY_TOOLTIP"
		,"UICMD_TITLE_SSID"
		,"UICMD_CHECK_LEFTBUT"
		,"UICMD_NUM"

            };
    for(int i=0;i<sizeof(cCombItems)/sizeof(cCombItems[0]);i++)
    {
        ui->comboBox->addItem(cCombItems[i]);
    }

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////

    const char * cParas[]={
		#include "routers.h"
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        ui->cob_model->addItem(cParas[i]);
    }
}

QGenieTestForm::~QGenieTestForm()
{
    delete ui;
}



void QGenieTestForm::on_uicmd_clicked()
{
    int cmd=ui->comboBox->currentIndex();
    QString stemp=ui->edt_para1->text();
    QVariant para1=string2variant(stemp);
    GENIE2_UI_COMMAND2(cmd,para1,0);
}

void QGenieTestForm::on_getsetting_clicked()
{
    ui->lab_setting->setText(GENIE2_GET_SETTING(ui->edt_setting->text()).toString());
}

void QGenieTestForm::on_setsetting_clicked()
{
    QString key=ui->edt_setting_set1->text();
    QString value=ui->edt_setting_set2->text();
    GENIE2_SET_SETTING(key,value);
}

void QGenieTestForm::on_subscrib_clicked()
{
    GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MAC);
}

void QGenieTestForm::on_notify_clicked()
{
    GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MAC,"ADAFDAFA");
}

void QGenieTestForm::on_but_quit_clicked()
{
    qApp->quit();
}

void QGenieTestForm::on_but_log_clicked()
{
    GENIE2_LOG(ui->edt_log->text());
}

void QGenieTestForm::on_but_routeranim_clicked()
{
    QGenieRouteWidget_B3 *w=new QGenieRouteWidget_B3();
    w->show();
}

void QGenieTestForm::on_but_currentsetting_clicked()
{
    INetgearInterface *netgearM=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM;
    netgearM->netgearCommand(INetgearInterface::IC_CURRENTSETTING,QStringList(),NULL,NULL);

}

void QGenieTestForm::on_but_test_clicked()
{
    QVariantList v;
    v<<-1<<0;
    GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,v);
    QVariant t=GENIE2_TELL_OTHERPLUGIN(FRAME_UUID,I2CMD_VERSIONNUMBER,0);
	qDebug()<<t;
}



void QGenieTestForm::on_cob_model_currentIndexChanged(QString s)
{
    QGenieIface2ShareObject::sSingleInstance->mDBGRouteModel=s;

    GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,s);
}
