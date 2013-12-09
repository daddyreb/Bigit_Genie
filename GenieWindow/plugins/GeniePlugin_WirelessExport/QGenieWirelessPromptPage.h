#ifndef QGENIEWIRELESSPROMPTPAGE_H
#define QGENIEWIRELESSPROMPTPAGE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include "QGenieProgressPanel.h"

class QGenieWirelessPromptPage : public QFrame
{
Q_OBJECT
public:
    explicit QGenieWirelessPromptPage(QWidget *parent = 0);
protected:
    QLabel                                      *welcome_image_lb;
    QLabel                                      *network_prompt_lb;
    QLabel                                      *ssid_lb;
    QPushButton                       *prev_btn;
    QPushButton                       *next_btn;
    QPushButton                       *cancel_btn;
    QGenieProgressPanel               *progress_panel;
protected:
    QString                                              m_strssid;
    QString                                              m_strpwd;
    QString                                              m_strnicguid;
    QString                                              m_strprofilename;
    bool                                                   m_bfrom_router;
    int                                                    m_networkprompt_lang_idx;
public:
    const QString & get_ssid();
    const QString & get_pwd();
    const QString & get_nicguid();
    const QString & get_profilename();
protected:
    virtual void changeEvent(QEvent *event);
    void retranslateUi();
public:
    void init(const QString &ssid, const QString &pwd);
    bool is_fromrouter();
signals:
    void wirelesspromptpage_prev_btn_clicked();
    void wirelesspromptpage_next_btn_clicked();
    void wirelesspromptpage_cancel_btn_clicked();
public slots:
     void getwlaninfo_complete(bool bok,const QString &ssid,const QString &key
                               ,const QString &guid,const QString &profilename);
     void cancel_btn_clicked();
};

#endif // QGENIEWIRELESSPROMPTPAGE_H
