#include "QGenieWirelessTab.h"
#include "plugin_ui_def.h"
#include <QPluginLoader>
#include <QButtonGroup>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "IManagerHandler.h"
#include "DiagnoseInterface.h"
 DiagnoseInterface                   *diagnose_inf = 0;
 extern IManagerHandler         *g_manager_handler;

QGenieWirelessTab::QGenieWirelessTab(QWidget *parent) :
    QWidget(parent)/*,m_internet_plugin(0)*/,m_wirelessexport_plugin(0),m_routerconfiguration_plugin(0)
   /*,m_internet_widget(0)*/ ,m_wirelessexport_widget(0),m_wchannel_view(0),m_vn_widget(0)
                             ,m_routerconfiguration_widget(0)
{
    QPluginLoader loader(QString("DiagnosePlugin.dll"));
    diagnose_inf=qobject_cast<DiagnoseInterface*>(loader.instance());

//    m_internet_btn                    = new QPushButton(this);
    m_wirelessexport_btn        = new QPushButton(this);
    m_wirelesschannel_btn      = new QPushButton(this);
    m_wirelessconnect_btn      = new QPushButton(this);
    m_routerconfiguration_btn  = new QPushButton(this);

    m_wirelessexport_btn->setCheckable(true);
    m_wirelesschannel_btn->setCheckable(true);
    m_wirelessconnect_btn->setCheckable(true);
    m_routerconfiguration_btn->setCheckable(true);

    m_tab_layout                      = new QStackedLayout();

    init_tab();

    m_wirelessexport_btn->setFocusPolicy(Qt::NoFocus);
    m_wirelesschannel_btn->setFocusPolicy(Qt::NoFocus);
    m_wirelessconnect_btn->setFocusPolicy(Qt::NoFocus);
    m_routerconfiguration_btn->setFocusPolicy(Qt::NoFocus);
//    SET_FIX_SIZE(TAB_LEFT,m_internet_btn);
     SET_FIX_SIZE(TAB_LEFT,m_wirelesschannel_btn);
     SET_FIX_SIZE(TAB_MID,m_wirelessconnect_btn);
    SET_FIX_SIZE(TAB_MID,m_wirelessexport_btn);
    SET_FIX_SIZE(TAB_RIGHT,m_routerconfiguration_btn);

    QButtonGroup     *btns_gp = new QButtonGroup(this);
    btns_gp->setExclusive(true);
//    btns_gp->addButton(m_internet_btn);
    btns_gp->addButton(m_wirelessexport_btn);
    btns_gp->addButton(m_wirelesschannel_btn);
    btns_gp->addButton(m_wirelessconnect_btn);
    btns_gp->addButton(m_routerconfiguration_btn);

    connect(btns_gp,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(btns_clicked(QAbstractButton*)));

    QHBoxLayout     *btns_layout = new QHBoxLayout();
    btns_layout->setSpacing(0);
    btns_layout->addStretch(1);
//    btns_layout->addWidget(m_internet_btn);
    btns_layout->addWidget(m_wirelesschannel_btn);
    btns_layout->addWidget(m_wirelessconnect_btn);
    btns_layout->addWidget(m_wirelessexport_btn);
    btns_layout->addWidget(m_routerconfiguration_btn);
    btns_layout->addStretch(1);

    QVBoxLayout     *ver_layout = new QVBoxLayout(this);
    ver_layout->addLayout(btns_layout);
    ver_layout->addLayout(m_tab_layout);
    ver_layout->setStretch(0,0);
    ver_layout->setStretch(1,1);

    retranslateUi();

    btns_clicked(m_wirelesschannel_btn);
}

void QGenieWirelessTab::init_tab()
{
//    QPluginLoader loader_internet(QString("GeniePlugin_Internet.dll"));
//   // loader.setFileName();
//    m_internet_plugin = qobject_cast<IPluginInf*>(loader_internet.instance());
//    if(m_internet_plugin)
//    {
//        m_internet_widget = m_internet_plugin->widget(this);
//        if(m_internet_widget)
//        {
//            m_tab_layout->addWidget(m_internet_widget);
//        }
//    }

    m_wchannel_view = new QGenieWirelessChannelView(this);
    m_tab_layout->addWidget(m_wchannel_view);

    m_vn_widget  = new  QGenieVisibleNetworkWidget(this);
    m_tab_layout->addWidget(m_vn_widget);

   QPluginLoader  loader(QString("InnerPlugin_WirelessExport.dll"));
    m_wirelessexport_plugin = qobject_cast<IPluginInf*>(loader.instance());
    if(m_wirelessexport_plugin)
    {
        m_wirelessexport_plugin->setHandler(g_manager_handler);
        m_wirelessexport_widget = m_wirelessexport_plugin->widget(this);
        if(m_wirelessexport_widget)
        {
            m_tab_layout->addWidget(m_wirelessexport_widget);
        }

    }

    //InnerPlugin_RouterConfiguration
    QPluginLoader  loader_rc(QString("InnerPlugin_RouterConfiguration.dll"));
     m_routerconfiguration_plugin = qobject_cast<IPluginInf*>(loader_rc.instance());
     if(m_routerconfiguration_plugin)
     {
         m_routerconfiguration_plugin->setHandler(g_manager_handler);
         m_routerconfiguration_widget = m_routerconfiguration_plugin->widget(this);
         if(m_routerconfiguration_widget)
         {
             m_tab_layout->addWidget(m_routerconfiguration_widget);
         }

     }
}

void QGenieWirelessTab::btns_clicked(QAbstractButton *btn)
{
/*    if(btn==m_internet_btn)
    {
        if(m_internet_widget)
            m_tab_layout->setCurrentWidget(m_internet_widget);
    }
    else*/ if(btn==m_wirelessexport_btn)
    {
        if(m_wirelessexport_widget)
       {
             m_tab_layout->setCurrentWidget(m_wirelessexport_widget);
       }

    }
    else if(btn==m_wirelesschannel_btn)
    {
        if(m_wchannel_view)
            m_tab_layout->setCurrentWidget(m_wchannel_view);
    }
    else if(btn==m_wirelessconnect_btn)
    {
        if(m_vn_widget)
            m_tab_layout->setCurrentWidget(m_vn_widget);
    }
    else if(btn == m_routerconfiguration_btn)
    {
        if(m_routerconfiguration_widget)
            m_tab_layout->setCurrentWidget(m_routerconfiguration_widget);
    }

    btn->setChecked(true);
}

void QGenieWirelessTab::retranslateUi()
{
//    if(m_internet_plugin)
//    {
//        m_internet_btn->setText(m_internet_plugin->text());
//    }

    if(m_wirelessexport_plugin)
    {
        m_wirelessexport_btn->setText(m_wirelessexport_plugin->text());
    }

    if(m_routerconfiguration_plugin)
    {
        m_routerconfiguration_btn->setText(m_routerconfiguration_plugin->text());
    }

    m_wirelesschannel_btn->setText(tr("Wireless Channel"));
    m_wirelessconnect_btn->setText(tr("Connect Wireless"));
}

void QGenieWirelessTab::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}
