#include "stable.h"
QGenieFrameDbgPluginInfo::QGenieFrameDbgPluginInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieFrameDbgPluginInfo)
{
    ui->setupUi(this);
    ui->label_version->setText(
                QString("Version:%1   Date:%2   Time:%3   repo verision:%4")
                .arg(VERSION_STRING,VERSTION_STR_DATE,VERSTION_STR_TIME,REPO_VERSION));
}

QGenieFrameDbgPluginInfo::~QGenieFrameDbgPluginInfo()
{
    delete ui;
}

void QGenieFrameDbgPluginInfo::on_tableWidget_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
    if(current)
    {
		ui->label->setText(current->data(Qt::DisplayRole).toString());
    }
}

