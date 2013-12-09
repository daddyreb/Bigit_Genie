#ifndef WIFICHANNELDATA_H
#define WIFICHANNELDATA_H

#include <QMap>
#include <QStringList>

struct WifiChannelData
{
    bool                   m_bautoChannel;
    int                    m_curChannel;
    int                    m_curSSidIndex;//current ssid's index in the per channel ssid list
    QMap<int, QStringList> m_ssidsPerChannel;

    WifiChannelData():m_bautoChannel(false),m_curChannel(-1),m_curSSidIndex(-1){}

    void reset()
    {
        m_bautoChannel  = false;
        m_curChannel    = -1;
        m_curSSidIndex  = -1;

        m_ssidsPerChannel.clear();
    }

};

#endif // WIFICHANNELDATA_H
