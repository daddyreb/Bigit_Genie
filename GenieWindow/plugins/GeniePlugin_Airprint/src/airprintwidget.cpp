#include "ui_airprintwidget.h"
#include "GeniePlugin_Airprint.h"
#include <QDebug>
#include "language_AirPrint.h"

AirprintWidget::AirprintWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AirprintWidget),
//        g_settings(NULL),
        uninstallationpixmap(NULL),
        airprintbackgroud(NULL),
        movie(new QMovie(":/res/icon_working.gif"))
//        uuid("{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b9}")
{
    ui->setupUi(this);

//    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->testpagerefreshbt);
//    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->printerlistpagerefreshbt);

    uninstallationpixmap = new QPixmap(tr(":/res/uninstallation.png"));
    ui->uninstallationimage->setPixmap(*uninstallationpixmap);

//    font = QFont("Arial",10, QFont::Bold);

//    if(GeniePlugin_Airprint::sHandler)
//        g_settings = GeniePlugin_Airprint::sHandler->getSettings();

    airprintbackgroud = new AirprintBackgroud();
    if(!airprintbackgroud)
    {
        return;
    }

//    if(GeniePlugin_Airprint::sHandler)
//        lanmanager = GeniePlugin_Airprint::sHandler->getLanguageManager();
    testpagestatus = qobject_cast<QProgressIndicator *>(ui->testpagestatus);
    printerlistpagestatus = qobject_cast<QProgressIndicator *>(ui->printerlistpagestatus);

    qRegisterMetaType<QMap<QString,bool> >("QMap<QString,bool>");
    qRegisterMetaType<Qt::CheckState>("Qt::CheckState");

    connect(airprintbackgroud,SIGNAL(sendMessage(QString,bool)),
            this,SLOT(recevieMessage(QString,bool)));
    connect(airprintbackgroud,SIGNAL(sendPrinterNameList(QMap<QString,bool>)),
            this,SLOT(receviePrinterNameList(QMap<QString,bool>)));
    connect(airprintbackgroud,SIGNAL(sendSaveConfig(QMap<QString,bool>)),
            this, SLOT(recevieSaveConfig(QMap<QString,bool>)));
    connect(airprintbackgroud,SIGNAL(sendStatus(short)),
            this,SLOT(recevieStatus(short)));
    connect(airprintbackgroud,SIGNAL(sendPrinterStatus(QString,Qt::CheckState)),
            this,SLOT(recevicePrinterStatus(QString,Qt::CheckState)));
    connect(airprintbackgroud,SIGNAL(sendNeedWriteConfig()),
            this,SLOT(receviceNeedWriteConfig()));

    connect(ui->printerlist,SIGNAL(itemClicked(QTableWidgetItem*)),
            this,SLOT(itemClicked(QTableWidgetItem*)));
    connect(ui->printerlistpagerefreshbt,SIGNAL(clicked()),
            this,SLOT(refeshBtClicked()));
    connect(ui->testpagerefreshbt,SIGNAL(clicked()),
            this,SLOT(refeshBtClicked()));

    connect(this,SIGNAL(sendLockMainWindow()),
            this,SLOT(receviceLockMainWindow()));
    connect(this,SIGNAL(sendUnlockMainWindow()),
            this,SLOT(receviceUnlockMainwindow()));

    airprintbackgroud->start();
    ui->stackedWidget->setCurrentWidget(ui->testpage);

    timerId = startTimer(10*60*1000);
}

AirprintWidget::~AirprintWidget()
{
    if(ui)
        delete ui;

    if(uninstallationpixmap)
        delete uninstallationpixmap;

    if(airprintbackgroud)
        delete airprintbackgroud;

    if(movie)
        delete movie;
}

void AirprintWidget::readConfig(QMap<QString, bool> &selectedprintermap)
{
//    if(g_settings)
//    {
        QString value = GENIE2_GET_SETTING(PLUGIN_AIRPRINT_PRINTERLIST).toString();
//                g_settings->value(PLUGIN_AIRPRINT_PRINTERLIST,\
//                                          PLUGIN_AIRPRINT_PRINTERLIST_DEFAULT).toString();
        if(!value.contains(","))
        {
            return;
        }
        QStringList printerlist = value.split(",");
        for(int i=0; i < printerlist.count(); i += 2)
        {
            selectedprintermap.insert(printerlist[i],printerlist[i+1].compare("true")==0?true:false);
        }
//    }
}

void AirprintWidget::writeConfig(QMap<QString, bool> selectedprintermap)
{
    qDebug() << "AirprintWidget::writeConfig" << selectedprintermap;
//    if(g_settings)
//    {
        QString value;
        QStringList valuelist;
        QMapIterator<QString,bool> it(selectedprintermap);
        while(it.hasNext())
        {
            it.next();
            valuelist << it.key();
            if(it.value())
            {
                valuelist << "true";
            }
            else
            {
                valuelist << "false";
            }
        }
        value = valuelist.join(",");
        GENIE2_SET_SETTING(PLUGIN_AIRPRINT_PRINTERLIST,value);
//        g_settings->setValue(PLUGIN_AIRPRINT_PRINTERLIST,value);
//    }
}

void AirprintWidget::receviceNeedWriteConfig()
{
    QMap<QString,bool> printernamemap;
    readConfig(printernamemap);
    qDebug() << printernamemap;
    QMapIterator<QString,bool> it(printernamemap);
    QList<QString> registerprinternamelist;
    QList<QString> unregisterprinternamelist;
    registerprinternamelist.clear();
    unregisterprinternamelist.clear();
    while(it.hasNext())
    {
        it.next();
        if(it.value())
        {
            registerprinternamelist << it.key();
        }else
        {
            unregisterprinternamelist << it.key();
        }
    }
    airprintbackgroud->setRegisterPrinterNames(registerprinternamelist);
    airprintbackgroud->setUnregisterPrinterNames(unregisterprinternamelist);
}

void AirprintWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

void AirprintWidget::retranslateUi()
{
//    if(GeniePlugin_Airprint::sHandler != NULL && lanmanager != NULL)
//    {
        ui->testpagerefreshbt->setText(GENIE2_GET_TEXT(L_AIRPRINT_REFRESHBT));

        ui->usedescription->setText(QString("%1<BR><BR>%2<BR>%3<BR><BR>%4<BR>%5")

                                    .arg(GENIE2_GET_TEXT(L_AIRPRINT_USAGEDESCRIPTION))
                                    .arg(GENIE2_GET_TEXT(L_AIRPRINT_USAGEDESCRIPTIONSTEP1TITLE))
                                    .arg(GENIE2_GET_TEXT(L_AIRPRINT_USAGEDESCRIPTIONSTEP1))
                                    .arg(GENIE2_GET_TEXT(L_AIRPRINT_USAGEDESCRIPTIONSTEP2TITLE))
                                    .arg(GENIE2_GET_TEXT(L_AIRPRINT_USAGEDESCRIPTIONSTEP2)));
//        ui->usedescription->setFont(font);
        ui->printerlistpagerefreshbt->setText(GENIE2_GET_TEXT(L_AIRPRINT_REFRESHBT));

        ui->refreshdescription->setText(GENIE2_GET_TEXT(L_AIRPRINT_REFRESHDESCRIPTION));
        ui->printerlistpagetitle->setText(GENIE2_GET_TEXT(L_AIRPRINT_RINTERLISTPAGETITLE));

        ui->uninstallationdescription->setText(QString("%1<BR>%2<BR>%3")
                                                   .arg(GENIE2_GET_TEXT(L_AIRPRINT_ALLUNINSTALLEDAPP))
                                                   .arg(GENIE2_GET_TEXT(L_AIRPRINT_ALLUNINSTALLEDAPP2))
                                                   .arg(GENIE2_GET_TEXT(L_AIRPRINT_ALLUNINSTALLEDAPP3)));
//        ui->uninstallationdescription->setFont(font);
//    }
}

void AirprintWidget::receviceLockMainWindow()
{
    ui->printerlist->setDisabled(true);
    ui->testpagerefreshbt->setDisabled(true);
    ui->printerlistpagerefreshbt->setDisabled(true);
    qDebug() << "AirprintWidget::receviceLockMainWindow()";
}

void AirprintWidget::receviceUnlockMainwindow()
{
    ui->printerlist->setDisabled(false);
    ui->testpagerefreshbt->setDisabled(false);
    ui->printerlistpagerefreshbt->setDisabled(false);
    qDebug() << "AirprintWidget::receviceUnlockMainwindow()";
}

void AirprintWidget::recevieStatus(short status)
{
    qDebug() << "AirprintWidget::recevieStatus" << status;

//    ui->uninstallationimage->clear();
//    ui->uninstallationdescription->clear();
//    ui->uninstallationimage2->clear();
//    ui->uninstallationdescription2->clear();

    //   switch(status)
    //    {
    //    case ADOBEREADERINSTALLED:
    //        {
    //            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
    //
    //            if(lanmanager != NULL)
    //                ui->uninstallationdescription->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDITUNES,uuid));
    //            ui->uninstallationdescription->setFont(font);
    //
    //            ui->uninstallationimage2->clear();
    //            ui->uninstallationdescription2->clear();
    //
    //            if(lanmanager != NULL)
    //                ui->refreshdescription->setText(lanmanager->getText(L_AIRPRINT_REFRESHDESCRIPTION,uuid));
    //
    //            ui->stackedWidget->setCurrentWidget(ui->testpage);
    //        }
    //        break;
    //    case ITUNESINSTALLED:
    //        {
    //            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
    //            if(lanmanager != NULL)
    //            ui->uninstallationdescription->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDADOBEREADER,uuid));
    //
    //            ui->uninstallationdescription->setFont(font);
    //
    //            ui->uninstallationimage2->clear();
    //            ui->uninstallationdescription2->clear();
    //            if(lanmanager != NULL)
    //            ui->refreshdescription->setText(lanmanager->getText(L_AIRPRINT_REFRESHDESCRIPTION,uuid));
    //
    //            ui->stackedWidget->setCurrentWidget(ui->testpage);
    //        }
    //        break;
    //    default:
    //        {
    //            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
    //            ui->uninstallationimage2->setPixmap(*uninstallationpixmap);
    //            if(lanmanager != NULL)
    //                ui->uninstallationdescription->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDITUNES,uuid));
    //            if(lanmanager != NULL)
    //                ui->uninstallationdescription2->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDADOBEREADER,uuid));
    //
    //            ui->uninstallationdescription->setFont(font);
    //            ui->uninstallationdescription2->setFont(font);
    //
    //            if(lanmanager != NULL)
    //            ui->refreshdescription->setText(lanmanager->getText(L_AIRPRINT_REFRESHDESCRIPTION,uuid));
    //
    //            ui->stackedWidget->setCurrentWidget(ui->testpage);
    //        }
    //        break;
    //    }

    switch(status)
    {
    case ALLINSTALLED:
        {
//            ui->uninstallationimage->clear();
//            ui->uninstallationimage2->clear();
//            ui->uninstallationdescription->clear();
//            ui->uninstallationdescription2->clear();

            ui->stackedWidget->setCurrentWidget(ui->printerlistpage);
        }
        break;
//    case ADOBEREADERINSTALLED:
//        {
//            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
//
//            if(lanmanager != NULL)
//                ui->uninstallationdescription->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDITUNES,uuid));
//            ui->uninstallationdescription->setFont(font);
//
//            ui->stackedWidget->setCurrentWidget(ui->testpage);
//        }
//        break;
//    case ITUNESINSTALLED:
//        {
//            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
//
//            if(lanmanager != NULL)
//                ui->uninstallationdescription->setText(lanmanager->getText(L_AIRPRINT_UNINSTALLEDADOBEREADER,uuid));
//            ui->uninstallationdescription->setFont(font);
//
//            ui->stackedWidget->setCurrentWidget(ui->testpage);
//        }
//        break;
    case ADOBEREADERINSTALLED:
    case ITUNESINSTALLED:
    default:
        {
//            ui->uninstallationimage->setPixmap(*uninstallationpixmap);
//
//            if(lanmanager != NULL)
//                ui->uninstallationdescription->setText(QString("%1<BR>%2<BR>%3")
//                                                       .arg(lanmanager->getText(L_AIRPRINT_ALLUNINSTALLEDAPP,uuid))
//                                                       .arg(lanmanager->getText(L_AIRPRINT_ALLUNINSTALLEDAPP2,uuid))
//                                                       .arg(lanmanager->getText(L_AIRPRINT_ALLUNINSTALLEDAPP3,uuid)));
//            ui->uninstallationdescription->setFont(font);

            ui->stackedWidget->setCurrentWidget(ui->testpage);
        }
    }
}

void AirprintWidget::receviePrinterNameList(QMap<QString, bool> printernamemap)
{
    int i=0;
    ui->printerlist->clear();
    ui->printerlist->setRowCount(printernamemap.count());
    printernamecheckboxitemmap.clear();
    QMapIterator<QString, bool> it(printernamemap);
    qDebug() << "AirprintWidget::receviePrinterNameList" << printernamemap;
    while(it.hasNext())
    {
        it.next();

        ui->printerlist->setColumnWidth(0,15);
        QTableWidgetItem *spaceitem2 = new QTableWidgetItem;
        ui->printerlist->setItem(i,0,spaceitem2);

        ui->printerlist->setColumnWidth(1,30);
        QTableWidgetItem *spaceitem = new QTableWidgetItem;
        ui->printerlist->setItem(i,1,spaceitem);
        spaceitem->setCheckState(Qt::Unchecked);
        printernamecheckboxitemmap.insert(it.key(),spaceitem);

        QTableWidgetItem *printernameitem = new QTableWidgetItem;
        printernameitem->setText(it.key());
//        printernameitem->setFont(font);
        ui->printerlist->setItem(i,2,printernameitem);

        spaceitem->setCheckState(it.value()?Qt::Checked:Qt::Unchecked);
        i++;
    }
}

void AirprintWidget::recevieSaveConfig(QMap<QString, bool> selectedprintermap)
{
    qDebug() << "AirprintWidget::recevieSaveConfig";
    writeConfig(selectedprintermap);
}

void AirprintWidget::recevicePrinterStatus(QString printername, Qt::CheckState status)
{
    if(printernamecheckboxitemmap.contains(printername))
    {
        printernamecheckboxitemmap.value(printername)->setCheckState(status);
    }
}

void AirprintWidget::itemClicked(QTableWidgetItem *i)
{
    if(!i)
    {
        return;
    }
    QTableWidgetItem * item = i->tableWidget()->item(i->row(),1);
    QTableWidgetItem * item2 = i->tableWidget()->item(i->row(),2);
    if(!item || !item2 || i != item)
    {
        return;
    }

    switch(item->checkState())
    {
    case Qt::Checked:
        {
            airprintbackgroud->setRegisterPrinterName(item2->text());
        }
        break;
    case Qt::Unchecked:
        {
            airprintbackgroud->setUnregisterPrinterName(item2->text());
        }
        break;
    default:
        {}
    }
}

void AirprintWidget::refeshBtClicked()
{
    airprintbackgroud->setCheckInstallationState();
}

void AirprintWidget::setMessage(QString &msg)
{
    ui->testpagemessagelabel->setText(msg);
    ui->printerlistpagemessagelabel->setText(msg);
}

void AirprintWidget::recevieMessage(QString msg, bool status)
{
    if(testpagestatus)status?testpagestatus->startAnimation():testpagestatus->stopAnimation();
    if(printerlistpagestatus)status?printerlistpagestatus->startAnimation():printerlistpagestatus->stopAnimation();
        status?emit sendLockMainWindow():emit sendUnlockMainWindow();;
        setMessage(msg);
        qDebug() << "AirprintWidget::recevieMessage"  << msg << status;
}

void AirprintWidget::timerEvent(QTimerEvent *timerEvent)
{
    static int i=0;
    i++;
    if (timerEvent->timerId() == timerId) {
        QList<QString> liststr;
        QTableWidgetItem * item;
        for(int i=0; i< ui->printerlist->rowCount();i++)
        {
            item = ui->printerlist->item(i,1);
            switch(item->checkState())
                   {
                   case Qt::Checked:
                       {
                            liststr << ui->printerlist->item(i,2)->text();
                       }
                       break;
                   default:
                       break;
                   }
        }
        if(liststr.count())
        {
            qDebug() << "time out " << QThread::currentThreadId() << liststr;
            airprintbackgroud->setCheckInstallationState();
        }
    }else
    {
        qDebug() << "not time out " << timerId;
    }
}
