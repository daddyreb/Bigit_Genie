#include "netcardspanel.h"
#include "networkmapview.h"

#include <QFile>
#include <QPushButton>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QtDebug>
#include <QSpacerItem>
#include "language_Map.h"
#include "genie2_interface.h"
#include "genie2_netgear.h"

#define DELAY_CURRENTSETTING 10 //seconds

#ifdef POWERLINE_REVISION

#include <QNetworkInterface>
#define PROCESS_FINISH_WAITTIME_MS 5000
#define MAX_INFPLC_ID 5
PowerlineHandler::PowerlineHandler()

{
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(slot_Timeout()));
    //    connect(&mProcess,SIGNAL(finished(int,QProcess::ExitStatus)),
    //            this,SLOT(finished(int,QProcess::ExitStatus)));
    mTimer.start(TIME_OUT);

}

PowerlineHandler::~PowerlineHandler()
{
    disconnect(&mTimer,SIGNAL(timeout()),0,0);
}

void PowerlineHandler::slot_Timeout()
{
    qDebug () << "PowerlineHandler::slot_Timeout()";

    QByteArray outputByteArray;

    if(QDir::current().exists(INT6K_PATH))
    {
        qDebug () << "int6k:" << QDir::current().absolutePath();

        mProcess.start(INT6K_PATH, QStringList() << "-r" << "all");
        if(mProcess.waitForFinished(PROCESS_FINISH_WAITTIME_MS))
        {
            outputByteArray = mProcess.readAllStandardOutput();
        }

        if(outputByteArray.isEmpty())
        {
            for(int infPLCId = 1 ; infPLCId <= MAX_INFPLC_ID ; ++infPLCId )
            {
                QString option("-i%1");
                mProcess.start(INT6K_PATH,QStringList() << "-r" << "all" << option.arg(infPLCId));
                if(mProcess.waitForFinished(PROCESS_FINISH_WAITTIME_MS))
                {
                    outputByteArray = mProcess.readAllStandardOutput();
                }

                if(!outputByteArray.isEmpty())
                {
                    break;
                }
            }
        }
    }

    emit powerLineStateChanged(outputByteArray.count('\n'));
}

//void PowerlineHandler::getInterfacesIndex(QList<int> &indexList)
//{
//    indexList.clear();

//    QList<QNetworkInterface> infList = QNetworkInterface::allInterfaces();
//    foreach(QNetworkInterface inf,infList)
//    {
//        if((inf.flags() & QNetworkInterface::IsUp) &&
//           (inf.flags() & QNetworkInterface::IsRunning) &&
//           (!(inf.flags() & QNetworkInterface::IsLoopBack)))
//        {
//            indexList.append(inf.index());
//        }
//    }

//    qDebug () << "getInterfacesIndex :" << indexList;
//}

void PowerlineHandler::slot_but_clicked()
{
    QString tmp1 = PROGRAM_PATH;
    QString tmp2 = PROGRAM_DIRCTORY;
    QByteArray tmp3 = qgetenv(ENVIROMENT);

    tmp1.insert(0, tmp3);
    tmp2.insert(0, tmp3);

    mProcess.startDetached(tmp1, QStringList(), tmp2);
}


//void PowerlineHandler::finished(int exitCode,QProcess::ExitStatus exitStatus)
//{
//    QByteArray byteArray = mProcess.readAllStandardOutput();
//    qDebug() << "PowerlineHandler::finished ,byteArray= " << byteArray;

//    emit powerLineStateChanged(!byteArray.isEmpty());
//}

void PowerlineHandler::launchUtitity()
{
    slot_but_clicked();
}

//PowerlineHandler g_powerLineHandler;

#endif


/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/





#define NETCARDSPANEL_STYLEFILE    "ui/netcardspanel.qss"

NetcardsPanel::NetcardsPanel(NetworkMapView *parent) :
    QWidget(parent),
    m_currentAdapterId(-1),
    m_currentAdapterNotifyed(false),
    m_netcardsCount(0),
    m_netcardAblityMask(0),
    m_netcardsVisible(true)
  #ifdef ENABLE_POWERLINE
  ,m_plcBtnUsible(false)
  ,m_backBtnUsible(false)
  #endif

{
    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true);

    connect(m_btnGroup,SIGNAL(buttonClicked(int)),
            this,SLOT(buttonClicked(int)));

    m_refreshBtn = new QPushButton(this);
    m_refreshBtn->setObjectName("RefreshMapBtn");;
    m_refreshBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_refreshBtn,SIGNAL(clicked()),this,SIGNAL(refreshClicked()));

#ifdef ENABLE_POWERLINE
    m_goPLCBtn = new QPushButton(this);
    m_goPLCBtn->setCheckable(true);
    m_goPLCBtn->setObjectName("GoPLCBtn");
    m_goPLCBtn->setFocusPolicy(Qt::NoFocus);
    m_goPLCBtn->setVisible(false);
    m_goPLCBtn->setEnabled(false);
    m_goPLCBtn->setChecked(false);
    m_btnGroup->addButton(m_goPLCBtn);

    m_backBtn = new QPushButton("Back", this);
    m_backBtn->setFocusPolicy(Qt::NoFocus);
    m_backBtn->setVisible(false);
    connect(m_backBtn, SIGNAL(clicked()),this, SIGNAL(backClicked()));
#endif

    QHBoxLayout *layout=preparePanelLayout();
    layoutBtns(layout);

    mDelayCurrrentSettingTimer.setSingleShot(true);
    connect(&mDelayCurrrentSettingTimer,SIGNAL(timeout())
            ,this,SLOT(slot_RefreshCurrentSetting()));

    applyStyle();
}

NetcardsPanel::~NetcardsPanel()
{
    disconnect(&mDelayCurrrentSettingTimer,SIGNAL(timeout()),NULL,NULL);
    mDelayCurrrentSettingTimer.stop();
}

void NetcardsPanel::applyStyle()
{
    QFile cssFile(GENIE2_RES(NETCARDSPANEL_STYLEFILE));
    if (cssFile.open(QIODevice::ReadOnly)) {

        QWidget *parentWidget = qobject_cast<QWidget*>(parent());
        if(parentWidget)
        {
            parentWidget->setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
        }
        else
        {
            setStyleSheet(QString::fromUtf8(cssFile.readAll().data()));
        }
    }
}

void NetcardsPanel::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        break;
    default:
        break;
    }
}
bool NetcardsPanel::verifyUsableNetcard()
{
    bool bok    = false;
    int  index  = 0;

    while(index < m_netcardsCount)
    {
        if(m_netcardBtnList[index]->isEnabled())
        {
            break;
        }
        ++index;
    }

    if( (bok = (index < m_netcardsCount)))
    {
        m_currentAdapterId = m_btnGroup->id(m_netcardBtnList[index]);

        //#ifdef ENABLE_POWERLINE
        //        if(!m_goPLCBtn->isEnabled() || !m_goPLCBtn->isChecked()){
        //#endif
        m_netcardBtnList[index]->setChecked(true);

        //#ifdef ENABLE_POWERLINE
        //        }
        //#endif
        notifyCurrentNetcardChanged();
    }
#ifdef ENABLE_POWERLINE
    else{
        m_goPLCBtn->setChecked(m_goPLCBtn->isEnabled());
        if(m_goPLCBtn->isEnabled()){
            emit requestGoPLCPage(true);
        }
    }
#endif

    return bok;
}

void NetcardsPanel::slot_RefreshCurrentSetting()
{
    int cmd=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
            ->netgearCommand(INetgearInterface::IC_CURRENTSETTING
                             ,QStringList(),NULL,NULL);
    qDebug()<<"refresh currentsetting"<<__FILE__<<__LINE__<<cmd;
}

void NetcardsPanel::onNetcardsChanged(const SVT_NotifyType &data)
{
    qDebug () << "onNetcardsChanged ,data = " << data;

    //reget current setting
    mDelayCurrrentSettingTimer.stop();
    mDelayCurrrentSettingTimer.start(DELAY_CURRENTSETTING*1000);


    NetworkMapView *netmapView = qobject_cast<NetworkMapView*>(parent());
    Q_ASSERT (netmapView);

    QHBoxLayout *panelLayout = preparePanelLayout();
    if(m_currentAdapterId == -1)
    {
        m_currentAdapterId = netmapView->getDefaultInterface();
        m_currentAdapterNotifyed = false;
    }

    bool oldCurNetcardUseable = false;

    m_netcardsCount = 0;
    m_netcardAblityMask = 0;

    for(int i = 0 ; i < data.size() ; ++i)
    {
        if(!data[i][IIE_ISVISUAL].compare("y",Qt::CaseInsensitive))
        {
            qDebug () << "Virtual network card: " << data[i][IIE_NAME];
            continue;
        }

        if(m_netcardBtnList.size() <= m_netcardsCount)
        {
            m_netcardBtnList.append(new QPushButton(this));
        }

        m_btnGroup->addButton(m_netcardBtnList[m_netcardsCount]);

        m_netcardBtnList[m_netcardsCount]->setVisible(m_netcardsVisible);

        if(data[i][IIE_WIRED].compare("y",Qt::CaseInsensitive))
        {
            m_netcardBtnList[m_netcardsCount]->setObjectName("WirelessNetCardBtn");
        }
        else
        {
            m_netcardBtnList[m_netcardsCount]->setObjectName("WiredNetCardBtn");
        }

        m_netcardBtnList[m_netcardsCount]->setFocusPolicy(Qt::NoFocus);
        m_netcardBtnList[m_netcardsCount]->setCheckable(true);
        m_netcardBtnList[m_netcardsCount]->setToolTip(data[i][IIE_NAME]);


        m_btnGroup->setId(m_netcardBtnList[m_netcardsCount],data[i][IIE_IDX].toInt());

        if((m_currentAdapterId == data[i][IIE_IDX].toInt()) &&
                !data[i][IIE_ISON].compare("y",Qt::CaseInsensitive))
        {
#ifdef ENABLE_POWERLINE
            if(!m_goPLCBtn->isEnabled() || !m_goPLCBtn->isChecked()){
#endif
                m_netcardBtnList[m_netcardsCount]->setChecked(true);

#ifdef ENABLE_POWERLINE
            }
#endif
            oldCurNetcardUseable = true;
        }

        m_netcardBtnList[m_netcardsCount]->setEnabled
                (!data[i][IIE_ISON].compare("y",Qt::CaseInsensitive));

        if(m_netcardBtnList[m_netcardsCount]->isEnabled())
        {
            m_netcardAblityMask |= (1 << m_netcardsCount);
        }


        panelLayout->addWidget(m_netcardBtnList[m_netcardsCount]);

        ++m_netcardsCount;
    }

    for(int k = m_netcardsCount ; k < m_netcardBtnList.size() ; ++k)
    {
        m_netcardBtnList[k]->setVisible(false);
        m_btnGroup->removeButton(m_netcardBtnList[k]);
    }

    layoutBtns(panelLayout);

    applyStyle();
    updateGeometry();

#ifdef ENABLE_POWERLINE
    if(m_goPLCBtn->isEnabled() && m_goPLCBtn->isChecked()){
        return;
    }
#endif

    if(oldCurNetcardUseable)
    {
        if(!m_currentAdapterNotifyed)
        {
            notifyCurrentNetcardChanged();
        }
    }
    else
    {
        verifyUsableNetcard();
    }
}

//void NetcardsPanel::setNetcardsVisible(bool visible)
//{
//    m_netcardsVisible = visible;

//    updateUiElements();
//}

#ifdef ENABLE_POWERLINE

void NetcardsPanel::updateUiElements()
{
    for(int idx = 0 ; idx < m_netcardsCount ; ++idx){
        m_netcardBtnList[idx]->setVisible(m_netcardsVisible);
    }


    m_goPLCBtn->setVisible(m_plcBtnUsible && (!m_backBtnUsible));

    m_refreshBtn->setVisible(!m_backBtnUsible);

    m_backBtn->setVisible(m_backBtnUsible);
}


void NetcardsPanel::setPLCBtnUsible(bool usible)
{
    m_plcBtnUsible = usible;
    m_goPLCBtn->setEnabled(m_plcBtnUsible);

    if(usible){
        if((m_netcardAblityMask == 0)){
            m_goPLCBtn->setChecked(true);
            emit requestGoPLCPage(true);
        }
    }else if(m_goPLCBtn->isChecked() && (m_btnGroup->button(m_currentAdapterId) != 0)){
        m_btnGroup->button(m_currentAdapterId)->setChecked(true);
        emit requestGoPLCPage(false);
    }

    updateUiElements();
}

void NetcardsPanel::setBackBtnUsible(bool usible)
{
    m_backBtnUsible = usible;
    m_netcardsVisible = !m_backBtnUsible;

    updateUiElements();
}

void NetcardsPanel::setBackBtnText(const QString &text)
{
    m_backBtn->setText(text);
}

#endif

QHBoxLayout *NetcardsPanel::preparePanelLayout()
{
    delete layout();
    QHBoxLayout *panelLayout = new QHBoxLayout(this);
    panelLayout->setContentsMargins(0,0,0,0);
    return panelLayout;
}

void NetcardsPanel::layoutBtns(QHBoxLayout *panelLayout)
{
    panelLayout->addSpacing(8);;
    panelLayout->addWidget(m_refreshBtn);

#ifdef ENABLE_POWERLINE
    panelLayout->insertWidget(0, m_goPLCBtn);
    panelLayout->addWidget(m_backBtn);
#endif
}

void NetcardsPanel::buttonClicked(int id)
{
#ifdef ENABLE_POWERLINE
    QPushButton *targetBtn = qobject_cast<QPushButton *>(m_btnGroup->button(id));
    if(targetBtn == m_goPLCBtn){
        emit requestGoPLCPage(true);
        return;
    }else{
        emit requestGoPLCPage(false);
    }
#endif

    if(m_currentAdapterId != id)
    {
        m_currentAdapterId = id;
        notifyCurrentNetcardChanged();
    }
}

void NetcardsPanel::notifyCurrentNetcardChanged()
{
    emit currentNetcardChanged(m_currentAdapterId);
    m_currentAdapterNotifyed = true;
}
