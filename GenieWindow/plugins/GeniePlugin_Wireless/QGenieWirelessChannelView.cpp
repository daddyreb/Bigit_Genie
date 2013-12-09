#include "QGenieWirelessChannelView.h"
#include "QGenieWirelessCommon.h"
#include <QVBoxLayout>
#include <QtAlgorithms>

QGenieWirelessChannelView::QGenieWirelessChannelView(QWidget *parent) :
        QFrame(parent)
{
    m_curchannel_widget = new QGenieCurChannelWidget(this);
    m_histogram_widget  = new QGenieHistogramWidget(this);

    QVBoxLayout  *ver_layout = new QVBoxLayout(this);
    ver_layout->setContentsMargins(1,2,1,1);
    ver_layout->setSpacing(6);
    ver_layout->addWidget(m_curchannel_widget);
    ver_layout->addWidget(m_histogram_widget);
    ver_layout->setStretch(0,0);
    ver_layout->setStretch(1,1);

    connect(m_curchannel_widget,SIGNAL(refresh_request()),this,SLOT(on_refresh_request()));
    connect(&m_refresh_thread,SIGNAL(refresh_complete(bool,WifiChannelData *)),this,SLOT(refresh_complete(bool,WifiChannelData *)));
    connect(m_histogram_widget,SIGNAL(viewCurrentPage(int)),m_curchannel_widget,SLOT(onViewCurrentPage(int)));

}

QGenieWirelessChannelView::~QGenieWirelessChannelView()
{
    m_refresh_thread.quit();
}


void QGenieWirelessChannelView::on_refresh_request()
{
    m_refresh_thread.begin_refresh(m_curchannel_widget->refresh_netgearrouter());
}

void QGenieWirelessChannelView::refresh_complete(bool bok, WifiChannelData *data)
{

    QStringList             tipsList;
    QList<unsigned int>     dataList;
    QList<int>              labelList;
    QString                 curSsid;
    int                     highlightDataIndex = -1;

    if( bok &&
       (data->m_curChannel > 0) &&
       (data->m_curSSidIndex >= 0) &&
       (data->m_curSSidIndex < data->m_ssidsPerChannel[data->m_curChannel].count()))
    {
        curSsid = data->m_ssidsPerChannel[data->m_curChannel][data->m_curSSidIndex];
    }

    buildChannelDataList(data,dataList);
    buildLabelDataList(data,labelList);
    buildSsids2Tooltips(data,tipsList);

    highlightDataIndex = labelList.indexOf(data->m_curChannel);

    m_curchannel_widget->loadData(bok,curSsid,data->m_curChannel,data->m_bautoChannel);
    m_histogram_widget->loadData(dataList,labelList,tipsList,highlightDataIndex);
}

void QGenieWirelessChannelView::showEvent(QShowEvent *event)
{
    m_curchannel_widget->begin_refresh();
    QWidget::showEvent(event);
}

void QGenieWirelessChannelView::buildChannelDataList(WifiChannelData *data,QList<unsigned int> &dataList)
{
    QList<int> keys = data->m_ssidsPerChannel.keys();
    qSort(keys);

    if(keys.count() > 0)
    {
        for(int m = 1;m <= CHANNELCOUNT_IN_2DOT4GHZFREQUENCY;++m)
        {
            dataList.append(keys.contains(m)?data->m_ssidsPerChannel[m].count():0);
        }
    }

    for(int k = 0;k < keys.count();++k)
    {
        if(keys[k] > CHANNELCOUNT_IN_2DOT4GHZFREQUENCY)
        {
            dataList.append(data->m_ssidsPerChannel[keys[k]].count());
        }
    }

//    //test
//    if(keys.count() > 0)
//        dataList.append(5);
}

void QGenieWirelessChannelView::buildLabelDataList(WifiChannelData *data, QList<int> &labelList)
{
    QList<int> keys = data->m_ssidsPerChannel.keys();
    qSort(keys);

    if(keys.count() > 0)
    {
        for(int m = 1;m <= CHANNELCOUNT_IN_2DOT4GHZFREQUENCY;++m)
        {
            labelList.append(m);
        }
    }

    for(int k = 0;k < keys.count();++k)
    {
        if(keys[k] > CHANNELCOUNT_IN_2DOT4GHZFREQUENCY)
        {
            labelList.append(keys[k]);
        }
    }

//    //test
//    if(keys.count() > 0)
//        labelList.append(36);
}


void QGenieWirelessChannelView::buildChannelTooltip(WifiChannelData *data, int channel, QString &strTooltip)
{
    for(int idx = 0;idx < data->m_ssidsPerChannel[channel].count();++idx)
    {
        if((data->m_curChannel != channel) || (idx != data->m_curSSidIndex))
        {
            if( strTooltip.isEmpty() )
            {
                strTooltip.append(QString("<span style=\" color:rgb(0,0,0);font-size:10pt;\"><b>") + data->m_ssidsPerChannel[channel][idx] + QString("</b></span>"));
            }
            else
            {
                strTooltip.append(QString("<br><span style=\" color:rgb(0,0,0);font-size:10pt;\"><b>") + data->m_ssidsPerChannel[channel][idx] + QString("</b></span>"));
            }
        }
    }

    if(data->m_curChannel == channel)
    {
        if( strTooltip.isEmpty() )
        {
            strTooltip.append(QString("<span style=\" color:rgb(0,113,215);font-size:10pt;\"><b>") + data->m_ssidsPerChannel[channel][data->m_curSSidIndex] + QString("</b></span>"));
        }
        else
        {
            strTooltip.append(QString("<br><span style=\" color:rgb(0,113,215);font-size:10pt;\"><b>") + data->m_ssidsPerChannel[channel][data->m_curSSidIndex] + QString("</b></span>"));
        }
    }
}

void QGenieWirelessChannelView::buildSsids2Tooltips(WifiChannelData *data,QStringList &tipsList)
{
    QList<int> keys = data->m_ssidsPerChannel.keys();
    qSort(keys);

    QString strTooltip;

    if(keys.count() > 0)
    {
        for(int m = 1;m <= CHANNELCOUNT_IN_2DOT4GHZFREQUENCY;++m)
        {
            strTooltip.clear();
            if(keys.contains(m))
            {
                buildChannelTooltip(data,m,strTooltip);
            }
            tipsList.append(strTooltip);
        }
    }

    for(int k = 0;k < keys.count();++k)
    {
        if(keys[k] > CHANNELCOUNT_IN_2DOT4GHZFREQUENCY)
        {
            strTooltip.clear();
            buildChannelTooltip(data,keys[k],strTooltip);
            tipsList.append(strTooltip);
        }
    }

//    //test
//    if(keys.count() > 0)
//        tipsList.append("test item");
}

void QGenieWirelessChannelView::enableChangeChannel(bool enable)
{
    m_curchannel_widget->enableChangeChannel(enable);
}
