#include "QGenieStaticInterface.h"
#include "ui_QGenieStaticInterface.h"
#include "QGenieDataContainner.h"

QGenieStaticInterface::QGenieStaticInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGenieStaticInterface)
{
    ui->setupUi(this);
    updateUi();
}

QGenieStaticInterface::~QGenieStaticInterface()
{
    delete ui;
}

void QGenieStaticInterface::updateUi()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    QList<QDate> dlst=container->getClickMap().keys();
    ui->comboBox->clear();
    foreach(QDate date,dlst)
    {
        ui->comboBox->addItem(date.toString("yyyy-MM-dd"),date);
    }

    int pagecount=ui->comboBox->count();
    if(pagecount>0)
    {
        ui->comboBox->setCurrentIndex(pagecount-1);
    }

    QString stemp;
    QMapIterator<QString,QString> i(container->getLocalInfoMap());

    while(i.hasNext())
    {
        i.next();
        stemp+=QString("%1 = %2\n").arg(i.key(),i.value());
    }
    ui->lab_linfo->setText(stemp);
    ui->lab_dir->setText(FILE_OF_SEARCHMAP);

    stemp="Search Str Map\n";
    QMap<QString,int> searchmap;
    QFile f(FILE_OF_SEARCHMAP);
	QString s = FILE_OF_SEARCHMAP;
    if(f.open(QIODevice::ReadOnly))
    {
        QDataStream in(&f);
        if(!in.atEnd())
        {
            in >> searchmap;
        }
    }
    f.close();
    QMapIterator<QString,int> isearch(searchmap);
    while(isearch.hasNext())
    {
        isearch.next();
        stemp+=QString("%1 = %2\n").arg(isearch.key(),QString::number(isearch.value()));
    }
    ui->lab_searchstrs->setText(stemp);

    updateRouteTypeData();


}
void QGenieStaticInterface::updateRouteTypeData()
{
    QAbstractItemModel *model=ui->tableWidgetRouteType->model();
    int allrows=model->rowCount();
    model->removeRows(0,allrows);

    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    foreach(tRouteInfo *p,container->mRoutTypeMap)
    {
        if(p)
        {
        int row=model->rowCount();
        model->insertRows(row,1);
        model->setData(model->index(row,0),p->routemac);
        model->setData(model->index(row,1),p->localmac);
        model->setData(model->index(row,2),p->modeln);
        model->setData(model->index(row,3),p->fversion);
        }

    }
}

void QGenieStaticInterface::on_comboBox_currentIndexChanged(int index)
{  
    QAbstractItemModel *model=ui->tableWidget->model();
    int allrows=model->rowCount();
    model->removeRows(0,allrows);
    if(index <0)
    {
        return;
    }
    QDate date=ui->comboBox->itemData(index).toDate();
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    QMap<QString,int > *map=container->getClickMap().value(date,NULL);


    if(map)
    {
        QMapIterator<QString,int> i(*map);
        while (i.hasNext()) {
            i.next();

            int row=model->rowCount();
            model->insertRows(row,1);
            QModelIndex index= model->index(row,0);
            model->setData(model->index(row,0),i.key());
            model->setData(model->index(row,1),container->mShortHashDetail.value(i.key()));
            model->setData(model->index(row,2),i.value());

        }
    }
}


void QGenieStaticInterface::on_sendclick_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->commitClickCount();
}

void QGenieStaticInterface::on_sendinstdata_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->commitInstallInfo();
}

void QGenieStaticInterface::on_sendrouterdata_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->commitRouterInfo();
}

void QGenieStaticInterface::on_sendsearch_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->commitSearchData();
}

void QGenieStaticInterface::on_pagebut_clicked()
{
    int n=ui->stackedWidget->count();
    int c=ui->stackedWidget->currentIndex ();
    ui->stackedWidget->setCurrentIndex((c+1+n)%n);
}

void QGenieStaticInterface::on_getinfo_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->getLocalInfo();
}

void QGenieStaticInterface::on_getrouteinfo_clicked()
{
    QGenieDataContainner *container=QGenieDataContainner::GetSingletonPtr();
    Q_ASSERT(container);
    container->recordRouteInfo();
}
