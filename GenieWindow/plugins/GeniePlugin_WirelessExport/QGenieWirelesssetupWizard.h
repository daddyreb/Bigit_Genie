#ifndef WIRELESSSETUPWIZARD_H
#define WIRELESSSETUPWIZARD_H

#include "QGenieWelcomePage.h"
#include "QGenieWirelessPromptPage.h"
#include "QGenieInsertFlashDiskPage.h"
#include "QGenieResultPage.h"

#include <QFrame>
#include <QStackedLayout>
#include <QString>

class QGenieWirelessSetupWizard : public QFrame
{
    Q_OBJECT
public:
    explicit QGenieWirelessSetupWizard(QWidget *parent = 0);

protected:
    QGenieWelcomePage                         *welcome_page;
    QGenieWirelessPromptPage                  *wprompt_page;
    QGenieInsertFlashDiskPage                 *ifd_page;
    QGenieResultPage                          *result_page;

    QStackedLayout                            *wizard_layout;
private:
    QString m_ssid;
    QString m_pwd;
    QString m_auth;
    QString m_cipher;

public:
    void SetExportData(const QString &ssid, const QString &pwd,
                       const QString &auth, const QString &cipher);
    void init();
signals:

public slots:
    void    on_welcomepage_next_btn_clicked();

    void    on_wirelesspromptpage_prev_btn_clicked();
    void    on_wirelesspromptpage_next_btn_clicked();

    void    on_ifdpage_prev_btn_clicked();
    void    on_ifdpage_next_btn_clicked();
    void    on_ifdpage_export_complete(bool bok);
    void    on_page_cancel_btn_clicked();

    void    on_resultpage_end_btn_clicked();
};

#endif // WIRELESSSETUPWIZARD_H
