#ifndef QGENIEAVAILABLENETWORKITEM_H
#define QGENIEAVAILABLENETWORKITEM_H

#include <QFrame>
#include <QLabel>
#include <QEvent>
#include <QString>

//static const char * const texts_to_tr[] =
//{
//    QT_TR_NOOP("Security enabled wireless network"),
//    QT_TR_NOOP("Security not enabled wireless network"),
//    QT_TR_NOOP("Please click \"Connect\" to connect to this network , other information may be needed."),
//    QT_TR_NOOP("Security not enabled,the information on this network maybe available to other people.")
//};

class QGenieAvailableNetworkItem : public QFrame
{
Q_OBJECT
public:
    explicit QGenieAvailableNetworkItem(QWidget *parent = 0);
protected:
    QLabel          *wireless_image_lb;
    QLabel          *ssid_text_lb;
    QLabel          *channel_text_lb;
    QLabel          *securityenabled_image_lb;
    QLabel          *securityprompt_text_lb;
    QLabel          *signal_image_lb;
    QLabel          *connectprompt_text_lb;
protected:
    QString         str_auth_algorithm;
    QString         str_encry_algorithm;
    bool              security_enabled;
    QString         str_bss_type;
    int                 signal_strength;
    int                 channel;
public:
    void set_ssid(const QString &ssid);
    QString get_ssid();
    void set_auth_algorithm(const QString &auth);
    QString get_auth_algorithm();
    void set_encry_algorithm(const QString &encry);
    QString get_encry_algorithm();
    void set_sec_enabled(bool enabled);
    bool get_sec_enabled();
    void set_bss_type(const QString &bss_type);
    void set_signal_strength(int strength);
    void set_channel(int ch);
protected:
    void retranslateUi();
protected:
    virtual void changeEvent(QEvent *event);
public:
    void set_selected(bool sel);
signals:

public slots:

};

#endif //QGENIEAVAILABLENETWORKITEM_H
