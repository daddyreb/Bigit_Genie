#include "profile.h"
#include <QMimeData>
#include "mainwindow.h"
#include <QClipboard>

static int a =1;
static int b =0;
//static int a =1;
//static int b =1;

Profile::Profile(QWidget* parent)
    :QWidget(parent)
{
    ComputerInfoMap = NULL;

    this->setProperty("stylehintplugin",5);

    gridLayout = new QGridLayout(this);

    groupBox = new QGroupBox(this);

    gridLayout_2 = new QGridLayout(groupBox);

    treeWidget = new QTreeWidget(groupBox);
    treeWidget->setObjectName("cpdetailsview");
//    treeWidget->setWordWrap(true);

    QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
    __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
    treeWidget->setHeaderItem(__qtreewidgetitem);
    treeWidget->setFrameShape(QFrame::NoFrame);
    treeWidget->setFrameShadow(QFrame::Raised);
    treeWidget->setHeaderHidden(true);
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    treeWidget->setColumnWidth(0,500);

    gridLayout_2->addWidget(treeWidget, 0, 0, 1, 3);

//    horizontalSpacer = new QSpacerItem(217, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//
//    gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);

    pushButton = new QPushButton(groupBox);
    pushButton->setFocusPolicy(Qt::NoFocus);
    pushButton->setProperty("BtnRefresh",1);

//    gridLayout_2->addWidget(pushButton, 1, 1, 1, 1);
//    gridLayout_2->addWidget(pushButton, 1, 0, 1, 1);
    gridLayout_2->addWidget(pushButton, 1, 1, 1, 1);

    pushButton_2 = new QPushButton(groupBox);
    pushButton_2->setFocusPolicy(Qt::NoFocus);
    //pushButton_2->setProperty("BtnSave",1);
    pushButton_2->setProperty("BtnCopy",1);

//    gridLayout_2->addWidget(pushButton_2, 1, 2, 1, 1);
//    gridLayout_2->addWidget(pushButton_2, 1, 1, 1, 1);
    gridLayout_2->addWidget(pushButton_2, 1, 2, 1, 1);

    pushButton_3 = new QPushButton(groupBox);
    pushButton_3->setFocusPolicy(Qt::NoFocus);
    pushButton_3->setProperty("BtnCopy",1);
    //////2010-10-15 Janion
    pushButton_3->setHidden(true);
    //gridLayout_2->addWidget(pushButton_3, 1, 2, 1, 1);

    gridLayout->addWidget(groupBox, 0, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    groupBox->setTitle(QString());
    pushButton->setText(QString(tr("Refresh")));
    pushButton_2->setText(QString(tr("Save")));
    pushButton_3->setText(QString(tr("Copy")));

    dragon = new DragonNetTool();
    connect(dragon, SIGNAL(GetSysInfoFinish()), this, SLOT(ViewSysInfo()));

    QFont font;
    font.setPointSize(10);
    treeWidget->setFont(font);

    connect(pushButton,SIGNAL(clicked()),this,SLOT(Refulsh()));
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(save()));
    connect(pushButton_3,SIGNAL(clicked()),this,SLOT(strcopy()));

    connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(TreeWidgetCpoy(QTreeWidgetItem*,int)));

//    connect(&thread,SIGNAL(GetInfoFinfish()),this,SLOT(ViewSysInfo()));

//    connect(treeWidget,SIGNAL(activated(QModelIndex)),this,SLOT(treeWidgetChange(QModelIndex)));

    connect(treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(treeWidgetExpaned(QTreeWidgetItem*)));

    connect(treeWidget,SIGNAL(itemCollapsed(QTreeWidgetItem*)),this,SLOT(treeWidgetCollapsed(QTreeWidgetItem*)));

    retranslateUi();
}
Profile::~Profile()
{

}
void Profile::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
        case QEvent::LanguageChange:
            retranslateUi();
            break;
        default:
                break;
    }
}
void Profile::retranslateUi()
{
    groupBox->setTitle(QString());
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_REFRESH));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_REFRESH,uuid));
    pushButton_2->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_SAVE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_SAVE,uuid));
    pushButton_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_COPY));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_COPY,uuid));
}
void Profile::GetSysInfo()
{
    if(b==0)
    {
		ComputerInfoMapNew.clear();
		dragon->GetSysInfoNew(ComputerInfoMapNew);
	}
}
void Profile::Refulsh()
{
    pushButton->setDisabled(true);

    //////2010-10-12 Jansion
    treeWidget->clear();

	ComputerInfoMapNew.clear();
	dragon->GetSysInfoNew(ComputerInfoMapNew);

    GENIE2_STATISTICS("h");
}

void Profile::saveas()
{

//    QFile file("Profile.txt");
//    file.open(QIODevice::WriteOnly);
//    QTextStream out(&file);
//    QString str,str1,str2;
//    QTreeWidgetItem* item = treeWidget->takeTopLevelItem(0);
//    QList<QTreeWidgetItem*>list =item->takeChildren();
//    for(int i =0;i<list.size();i++)
//    {
//        str =list.at(i)->text(0).trimmed();
//        out << str +"\r";
//        int count = list.at(i)->childCount();
//        for(int j =0; j<count;j++)
//        {
//            str1 =list.at(i)->child(j)->text(0).trimmed();
//            out << str1 +"\r";
//            int size =list.at(i)->child(j)->childCount();
//            for(int k=0; k<size;k++)
//            {
//                str2 =list.at(i)->child(j)->child(k)->text(0).trimmed();
//                out << str2 +"\r";
//            }
//        }
//    }
//    file.close();


    a=1;
//    Refulsh();
}

void Profile::ViewSysInfo()
{
    treeWidget->clear();
    TreeRootItem = new QTreeWidgetItem(treeWidget);
    TreeRootItem->setText(0, "Your Computer Details");

    QMap<QString, QMap<QString, QStringList> >::const_iterator p_NodeData = ComputerInfoMapNew.constBegin();
    while (p_NodeData != ComputerInfoMapNew.constEnd()) {
        p_nodeDetail = new QTreeWidgetItem(TreeRootItem);
        p_nodeDetail->setText(0,p_NodeData.key());
        if(p_NodeData.key() =="NetWork Interface")
            p_nodeDetail->setText(0, "Network Adapters");
//        if(p_NodeData.key() =="Win32_Processor")
//            p_nodeDetail->setText(0, "CPU");
        if(p_NodeData.key() =="Win32_OperatingSystem")
            p_nodeDetail->setText(0, "Operating System Version");
//        if(p_NodeData.key() =="Win32_LogicalDisk")
//            p_nodeDetail->setText(0, "Logical Hard Drives");
//        if(p_NodeData.key() =="Win32_NetworkClient")
//            p_nodeDetail->setText(0, "Network Adapters");
//        if(p_NodeData.key() =="Win32_PhysicalMemory")
//            p_nodeDetail->setText(0, "System Memory Status");

        QMap<QString, QStringList> p_NodeValue;
        p_NodeValue = p_NodeData.value();
        QMap<QString, QStringList>::const_iterator cc_NodeData = p_NodeValue.constBegin();

        if (p_NodeValue.count() == 1){
            QStringList pp_NodeValue;
            pp_NodeValue = cc_NodeData.value();

            QStringList::const_iterator childNode = pp_NodeValue.constBegin();
            QString c_NodeData;
            for (int i=0; i<pp_NodeValue.size(); i++){
                c_NodeData.clear();
                if ( i+1 < pp_NodeValue.size() )
//                    c_NodeData = pp_NodeValue[i] + ": " + pp_NodeValue[i+1] + "\n";
                    c_NodeData = pp_NodeValue[i] + ": " + pp_NodeValue[i+1];
                else
//                    c_NodeData = pp_NodeValue[i] + "\n";
                    c_NodeData = pp_NodeValue[i];
                c_nodeDetail = new QTreeWidgetItem(p_nodeDetail);
                c_nodeDetail->setText(0, c_NodeData);

                i++;
            }
        }else {
            while (cc_NodeData != p_NodeValue.constEnd()){
                c_nodeDetail = new QTreeWidgetItem(p_nodeDetail);
                c_nodeDetail->setText(0, cc_NodeData.key());


                QStringList mp_NodeValue;
                mp_NodeValue = cc_NodeData.value();
                QStringList::const_iterator childNode = mp_NodeValue.constBegin();
                QString mc_NodeData;
                for (int j=0; j<mp_NodeValue.size(); j++){
                    mc_NodeData.clear();
                    if ( j+1 < mp_NodeValue.size() )
//                        mc_NodeData = mp_NodeValue[j] + ": " + mp_NodeValue[j+1] + "\n";
                        mc_NodeData = mp_NodeValue[j] + ": " + mp_NodeValue[j+1];
                    else
//                        mc_NodeData = mp_NodeValue[j] + "\n";
                        mc_NodeData = mp_NodeValue[j];
                    cc_nodeDetail = new QTreeWidgetItem(c_nodeDetail);
                    cc_nodeDetail->setText(0, mc_NodeData);

                    j++;
                }
                 ++cc_NodeData;
            }
        }
         ++p_NodeData;
     }
    treeWidget->expandItem(TreeRootItem);
    pushButton->setDisabled(false);
    b=1;
    if(a==0)
    {
        saveas();
        b=0;
    }
    treeWidget->setColumnWidth(0,150);
}
void Profile::TreeWidgetCpoy(QTreeWidgetItem * item, int column)
{
    line = item->text(column);
    qDebug()<<line;
}

void Profile::save()
{
    //////told server to const
    GENIE2_STATISTICS("N5");

    QString fileName = QFileDialog::getSaveFileName(this
        ,GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPROFILE_SAVE)
        ,"../profile.txt"
        ,tr("Normal text file (*.txt)"));
    if(!fileName .isNull())
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        QString str,str1,str2;

        //////2010-10-12 Jansion
//        QTreeWidgetItem* item = treeWidget->takeTopLevelItem(0);
//        QList<QTreeWidgetItem*>list =item->takeChildren();
//        for(int i =0;i<list.size();i++)
//        {
//            str =list.at(i)->text(0).trimmed();
//            out <<"Your Computer Details:\r" + str +"\r";
//            int count = list.at(i)->childCount();
//            for(int j =0; j<count;j++)
//            {
//                str1 =list.at(i)->child(j)->text(0).trimmed();
//                out << str1 +"\r";
//                int size =list.at(i)->child(j)->childCount();
//                for(int k=0; k<size;k++)
//                {
//                    str2 =list.at(i)->child(j)->child(k)->text(0).trimmed();
//                    out << str2 +"\r";
//                }
//            }
//        }

        QTreeWidgetItem* item = treeWidget->topLevelItem(0);
        QTreeWidgetItem* listItem;
        for(int i=0; i<item->childCount(); i++){
            listItem = item->child(i);

            str = listItem->text(0).trimmed();
            out <<"Your Computer Details:\r" + str +"\r";
            int count =  listItem->childCount();
            for(int j =0; j<count;j++)
            {
                str1 = listItem->child(j)->text(0).trimmed();
                out << str1 +"\r";
                int size = listItem->child(j)->childCount();
                for(int k=0; k<size;k++)
                {
                    str2 = listItem->child(j)->child(k)->text(0).trimmed();
                    out << str2 +"\r";
                }
            }

        }
        //////

        file.close();
    }

    //////2010-10-11 Jansion
    //Refulsh();

    treeWidget->setColumnWidth(0,150);

    //QString s ="h";
    //IClickManager* clickmanage =MainWindow::sHandler->getClickManager();
    //clickmanage->click(s);
}

void Profile::strcopy()
{
    QString str,str1,str2;
    QString txtpro = "Your Computer Details: \r";

    //////2010-10-11 Jansion
//    QTreeWidgetItem* item = treeWidget->takeTopLevelItem(0);
//    QList<QTreeWidgetItem*>list =item->takeChildren();
//    for(int i =0;i<list.size();i++)
//    {
//        str =list.at(i)->text(0).trimmed();
//        txtpro = txtpro + str + "\r";
//        int count = list.at(i)->childCount();
//        for(int j =0; j<count;j++)
//        {
//            str1 =list.at(i)->child(j)->text(0).trimmed();
//            txtpro = txtpro + str1 + "\r";
//            int size =list.at(i)->child(j)->childCount();
//            for(int k=0; k<size;k++)
//            {
//                str2 =list.at(i)->child(j)->child(k)->text(0).trimmed();
//                txtpro  = txtpro + str2 + "\r";
//            }
//        }
//    }

    QTreeWidgetItem* item = treeWidget->topLevelItem(0);
    QTreeWidgetItem* listItem;
    for(int i=0; i<item->childCount(); i++){
        listItem = item->child(i);
        str =listItem->text(0).trimmed();
        txtpro = txtpro + str + "\r";
        int count = listItem->childCount();
        for(int j =0; j<count;j++)
        {
            str1 =listItem->child(j)->text(0).trimmed();
            txtpro = txtpro + str1 + "\r";
            int size =listItem->child(j)->childCount();
            for(int k=0; k<size;k++)
            {
                str2 =listItem->child(j)->child(k)->text(0).trimmed();
                txtpro  = txtpro + str2 + "\r";
            }
        }
    }
    //////
    QApplication::clipboard()->setText(txtpro);
    qDebug()<<line;

    //////2010-10-11 Jansion
    //Refulsh();

    treeWidget->setColumnWidth(0,150);
}
void Profile::treeWidgetExpaned(QTreeWidgetItem * item)
{
	QTreeWidgetItem* topitem =treeWidget->topLevelItem(0);
	int count = topitem->child(0)->childCount();
	for(int i =0; i< count;i++)
	{
		if(topitem->child(0)->child(i) ==item)
		{
			treeWidget->setColumnWidth(0,500);
        }
        if(topitem->child(0) ==item)
        {
            if(topitem->child(0)->child(i)->isExpanded())
            {
                treeWidget->setColumnWidth(0,500);
            }
        }
        if(topitem == item)
        {
            if(topitem->child(0)->child(i)->isExpanded())
            {
                treeWidget->setColumnWidth(0,500);
            }
        }
    }
}
void Profile::treeWidgetCollapsed(QTreeWidgetItem *item)
{
    QTreeWidgetItem* topitem =treeWidget->topLevelItem(0);
    int count = topitem->child(0)->childCount();
    for(int i =0; i< count;i++)
    {
        if(topitem->child(0)->child(i) ==item)
        {
            if(i>0 && i<count)
            {
                if(topitem->child(0)->child(i)->isExpanded() | topitem->child(0)->child(i-1)->isExpanded())
                {
                    treeWidget->setColumnWidth(0,500);
                }
                else
                {
                    treeWidget->setColumnWidth(0,150);
                }
                if(topitem->child(0) ==item)
                {
                    treeWidget->setColumnWidth(0,150);
                }
                if(topitem == item)
                {
                    treeWidget->setColumnWidth(0,150);
                }
            }
            if(i ==0)
            {
                if(topitem->child(0)->child(1)->isExpanded() | topitem->child(0)->child(0)->isExpanded())
                {
                    treeWidget->setColumnWidth(0,500);
                }
                else
                {
                    treeWidget->setColumnWidth(0,150);
                }
                if(topitem->child(0) ==item)
                {
                    treeWidget->setColumnWidth(0,150);
                }
                if(topitem == item)
                {
                    treeWidget->setColumnWidth(0,150);
                }
            }
        }
    }
    if(topitem->child(0) ==item)
    {
        treeWidget->setColumnWidth(0,150);
    }
    if(topitem == item)
    {
        treeWidget->setColumnWidth(0,150);
    }
}
GetInfoThread::GetInfoThread(QObject *parent)
    :QThread(parent)
{
  //  connect(&dragon,SIGNAL(GetSysInfoFinish()),this,SIGNAL(GetInfoFinfish()));
}
GetInfoThread::~GetInfoThread()
{
    this->wait();
}
void GetInfoThread::begin_GetInfo(QMap<QString, QMap<QString, QStringList> > *Info)
{
    InfoMap = Info;
    this->start();
}
void GetInfoThread::run()
{
   // dragon.GetSysInfo(&InfoMap);
}
