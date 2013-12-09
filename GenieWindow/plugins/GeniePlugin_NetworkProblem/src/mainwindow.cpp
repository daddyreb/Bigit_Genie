#include "mainwindow.h"

#define TAB_STR "  %1  "

MainWindow::MainWindow()
{
    this->setProperty("stylehintplugin",5);

    gridLayout = new QGridLayout(this);

    tabWidget = new QTabWidget(this);
    tabWidget->setFocusPolicy(Qt::NoFocus);
#ifdef Q_OS_MAC
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(slot_ClearDirtyFocusFrame()));
#endif

    tab = new QWidget();
    tab->setFocusPolicy(Qt::NoFocus);
    stackedLayout = new QStackedLayout(tab);

    faq = new FAQ(tab);
    stackedLayout->addWidget(faq);
    tabWidget->addTab(tab, QString());

    tab_2 = new QWidget();
    tab_2->setFocusPolicy(Qt::NoFocus);
    stackedLayout_2 = new QStackedLayout(tab_2);
    tools = new Tools(tab_2);
#ifdef Q_OS_MAC
    connect(tools->stackedLayout,SIGNAL(currentChanged(int)),this,SLOT(slot_ClearDirtyFocusFrame()));
#endif
    stackedLayout_2->addWidget(tools);
    tabWidget->addTab(tab_2, QString());

    tab_3 = new QWidget();
    tab_3->setFocusPolicy(Qt::NoFocus);
    stackedLayout_3 = new QStackedLayout(tab_3);
    sp = new Support(tab_3);
    stackedLayout_3->addWidget(sp);
    tabWidget->addTab(tab_3, QString());

#ifdef Q_OS_MAC
    gridLayout->setContentsMargins(12,20,12,12);
#endif

    gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    tabWidget->setTabText(tabWidget->indexOf(tab), tr("FAQ"));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), tr("Tools"));
    tabWidget->setTabText(tabWidget->indexOf(tab_3), tr("Support"));

    connect(this,SIGNAL(getprofile()),tools,SLOT(getinfoprofile()));

    retranslateUi();

    //emit getprofile();

}
MainWindow::~MainWindow()
{
//    delete MainWindow::sHandler;
}
void MainWindow::changeEvent(QEvent* event)
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

void MainWindow::slot_ClearDirtyFocusFrame()
{
    GENIE2_UI_COMMAND(UICMD_CLEAR_DIRTY_FOCUSFRAME_FOR_MACOS,0);
}

void MainWindow::retranslateUi()
{
    QString strTemp;
    strTemp = QString(TAB_STR).arg(GENIE2_GET_TEXT(L_NETWORKPROBLEM_FAQ));//lanmanager->getText(L_NETWORKPROBLEM_FAQ,uuid));
    tabWidget->setTabText(tabWidget->indexOf(tab), strTemp);
    strTemp = QString(TAB_STR).arg(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS,uuid));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), strTemp);
    strTemp = QString(TAB_STR).arg(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT,uuid));
    tabWidget->setTabText(tabWidget->indexOf(tab_3), strTemp);

    //tabWidget->setTabText(tabWidget->indexOf(tab),lanmanager->getText(L_NETWORKPROBLEM_FAQ,uuid));
    //tabWidget->setTabText(tabWidget->indexOf(tab_2),lanmanager->getText(L_NETWORKPROBLEM_TOOLS,uuid));
    //tabWidget->setTabText(tabWidget->indexOf(tab_3),lanmanager->getText(L_NETWORKPROBLEM_SUPPORT,uuid));
}
