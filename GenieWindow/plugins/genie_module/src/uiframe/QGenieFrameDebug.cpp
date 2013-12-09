#include "stable.h"

const char * cParas[]={
    "",
    "Route Type CG3300",
    "Route Type CGD24G",
    "Route Type DG834",
    "Route Type DG834G",
    "Route Type DG834GT",
    "Route Type DG834GV",
    "Route Type DG834N",
    "Route Type DG834PN",
    "Route Type DGN1000_RN",
    "Route Type DGN2000",
    "Route Type DGN2200",
    "Route Type DGN2200M",
    "Route Type DGN3500",
    "Route Type DGND3300",
    "Route Type DM111P",
    "Route Type DM111PSP",
    "Route Type JWNR2000",
    "Route Type MBM621",
    "Route Type MBR1210-1BMCNS",
    "Route Type MBR624GU",
    "Route Type MBRN3000",
    "Route Type RP614",
    "Route Type WGR612",
    "Route Type WGR614",
    "Route Type WGR614L",
    "Route Type WGT624",
    "Route Type WNR2000",
    "Route Type WNDR3300",
    "Route Type WNDR3400",
    "Route Type WNDR3700",
    "Route Type WNDR37AV",
    "Route Type WNR1000",
    "Route Type WNR2200",
    "Route Type WNR3500",
    "Route Type WNR3500L",
    "Route Type WNR612",
    "Route Type WNR834B",
    "Route Type WNR834M",
    "Route Type WNR854T",
    "Route Type WPN824",
    "Route Type WPN824EXT",
    "Route Type WPN824N",

    "Route Type DGNB2100",
    "Route Type DGND3700",
    "Route Type WNB2100",
    "Route Type WNDR3800",
    "Route Type WNDR4000",
    "Route Type WNXR2000",
    "Route Type NETGEAR",
    "Route Type WNDRMAC"

};

QGenieFrameDebug::QGenieFrameDebug(QWidget *parent)
    :QFrame(parent)
{
    mUi.setupUi(this);
    QGenieBigButtonManager *manager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(manager);
    connect(mUi.pushButton_0,SIGNAL(pressed()),this,SLOT(slot_but0()));
    connect(mUi.pushButton_1,SIGNAL(pressed()),this,SLOT(slot_but1()));
    connect(mUi.pushButton_2,SIGNAL(pressed()),this,SLOT(slot_but2()));
    connect(mUi.pushButton_3,SIGNAL(pressed()),this,SLOT(slot_but3()));
    connect(mUi.pushButton_4,SIGNAL(pressed()),this,SLOT(slot_but4()));
    connect(mUi.pushButton_5,SIGNAL(pressed()),this,SLOT(slot_but5()));
    connect(mUi.pushButton_6,SIGNAL(pressed()),this,SLOT(slot_but6()));
    connect(mUi.pushButton_7,SIGNAL(pressed()),this,SLOT(slot_but7()));
    connect(mUi.pushButton_8,SIGNAL(pressed()),this,SLOT(slot_but8()));
    connect(mUi.pushButton_9,SIGNAL(pressed()),this,SLOT(slot_but9()));
    connect(mUi.pushButton_10,SIGNAL(pressed()),this,SLOT(slot_but10()));
    connect(mUi.pushButton_11,SIGNAL(pressed()),this,SLOT(slot_but11()));

//    connect(this,SIGNAL(signal_SetButState(int,int)),manager,SLOT(slot_SetState(int,int)));

    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        mUi.cob_routetype->addItem(cParas[i]);
    }
    mUi.cob_routetype->setCurrentIndex(0);
}

void QGenieFrameDebug::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);

        event->accept();
    }
//    qDebug("mouse move");
}


void QGenieFrameDebug::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void	QGenieFrameDebug::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
        qDebug("mouse release");
        mDrag=false;
        event->accept();
    }

}


void QGenieFrameDebug::slot_but0()
{
	static int sState=0;


}

void QGenieFrameDebug::slot_but1()
{

}

void QGenieFrameDebug::slot_but2()
{
    QGenieStyleManager *manager=QGenieStyleManager::GetSingletonPtr();
    ASSERT(manager);
    manager->setStyle(EST_TEST  );

}

void QGenieFrameDebug::slot_but3()
{
//        QGenieBigButtonManager *manager=QGenieBigButtonManager::GetSingletonPtr();
//        ASSERT(manager);
//        manager->loadStyleSheet();

}
void QGenieFrameDebug::slot_but4()
{
//    QGenieFrameAD *manager=QGenieFrameAD::GetSingletonPtr();
//    ASSERT(manager);
//    QString s=mUi.lineEdit->text();
//    qDebug("change page to %s",s.toLatin1().data());
    //manager->changePage(s);

}

void QGenieFrameDebug::slot_but5()
{
    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
    static bool btemp=false;
    btemp=!btemp;
    title->setSoftWareUpdateState(!btemp);
    title->setFirmwareUpdateState(btemp);


    //panel->checkButton(temp);
}

void QGenieFrameDebug::slot_but6()
{
    QString s="testmessagebox";
    //s=s.arg(fileName,parsor.mResult);
    QMessageBox msgBox;
     msgBox.setText(s);
     msgBox.exec();

}

void QGenieFrameDebug::slot_but7()
{
    QGenieTrayManager *tray=QGenieTrayManager::GetSingletonPtr();
    ASSERT(tray);
    tray->showMsg("title","test message",QSystemTrayIcon::Warning);
}

void QGenieFrameDebug::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
        mUi.retranslateUi(this);
    }
    QFrame::changeEvent(event);
}

void QGenieFrameDebug::slot_but8()
{
    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);
    lanmanager->setLanguage(ELT_CHINESE);

}

void QGenieFrameDebug::slot_but9()
{
//unload pi

    QString filename=mUi.comboBox->currentText();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    proxymanager->unloadPi(filename);


}

void QGenieFrameDebug::slot_but10()
{
    QString filename=mUi.comboBox->currentText();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    proxymanager->loadPi(filename);
}

void QGenieFrameDebug::slot_but11()
{
    qDebug()<<"QGenieFrameDebug::slot_but11()";

    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uimanager);
    static int s=0;
    s=(s+1)%(sizeof(cParas)/sizeof(cParas[0]));
    uimanager->command(UI_CMD_SET_ROUTE,cParas[s]);

}

void QGenieFrameDebug::on_but1_clicked()
{
    qDebug()<<"QGenieFrameDebug::on_but1_clicked()";
    const char * cParas[]={
        "",
        "Route Type WNDR3400",
        "Route Type WNDR3700",
    };
    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uimanager);
    static int s=0;
    s=(s+1)%(sizeof(cParas)/sizeof(cParas[0]));
    uimanager->command(UI_CMD_SET_ROUTE,cParas[s]);

}
void QGenieFrameDebug::on_but_crash_clicked()
{
    QGenieInterfaceUiManager *uimanager=NULL;
    uimanager->changeStackIdx(0);
}

    void QGenieFrameDebug::on_but1_2_clicked()
    {
        qApp->setLayoutDirection(Qt::LeftToRight);
    }


void QGenieFrameDebug::on_but1_3_clicked()
{
    qApp->setLayoutDirection(Qt::RightToLeft);
}

void QGenieFrameDebug::on_cob_routetype_currentIndexChanged(QString s)
{
    QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uimanager);
    uimanager->command(UI_CMD_SET_ROUTE,s);
}

void QGenieFrameDebug::on_but_gacc_timer_clicked()
{
    //QPushButton *but=qobject_cast<QPushButton*> (sender());
    //QGeniePluginProxyManager *pluginmanager=QGeniePluginProxyManager::GetSingletonPtr();
    //ASSERT(pluginmanager);
    //plug_inf *p=pluginmanager->getPluginInf(PLUGIN_UUID_NETCONFIG);
    //if(but && p && p->pi)
    //{
    //    but->setText(p->pi->getInfo("gacctimer"));
    //}
}

void QGenieFrameDebug::on_but_dbg_plugin_clicked()
{
    QString plugin_file_name=mUi.comboBox->currentText();
    QGeniePluginProxyManager *plugin_inf_manager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(plugin_inf_manager);
    plug_inf *p=plugin_inf_manager->mInfoManager.mFileNameIdx.value(plugin_file_name);

    QString para=mUi.dbg_para->text();
    if(p && p->pi)
    {
        p->pi->command(COMMAND_DEBUG,para);
    }
}
