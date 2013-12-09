#ifndef  QGENIEINSERTFLASHDISKPAGE_H
#define QGENIEINSERTFLASHDISKPAGE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QTimer>
#include "QGenieExport2DiskFileThread.h"


class QGenieInsertFlashDiskPage : public QFrame
{
Q_OBJECT
public:
    explicit QGenieInsertFlashDiskPage(QWidget *parent = 0);
protected:
    QLabel                                           *opt_prompt_lb;
    QLabel                                           *opt_image_lb;
    QPushButton                                 *prev_btn;
    QPushButton                                  *next_btn;
    QPushButton                                  *cancel_btn;
protected:
    QTimer                                          discover_flash_timer;
    QString                                          cur_flash_disk;
protected:
    QString                                         m_nic_guid;
    QString                                         m_ssid;
    QString                                         m_profile_name_or_pwd;
    QString                                         m_auth;
    QString                                         m_cipher;
    QGenieExport2DiskFileThread    m_export_thread;
    bool                                              m_bexporting;
    int                                                 m_opt_prompt_lb_lang_idx;
    bool                                              m_bdicoverflash_flag;
    QStringList                                   m_removable_list_prev;
protected:
    virtual void changeEvent(QEvent *event);
    void retranslateUi();
public:
    void init(const QString &nic_guid,const QString &ssid,const QString &profile_name_or_pwd
              ,const QString &auth, const QString &cipher);
signals:
    void ifdpage_next_btn_clicked();
    void ifdpage_prev_btn_clicked();
    void ifdpage_cancel_btn_clicked();

    void ifdpage_export_complete(bool bsuccess);
public slots:
    void on_next_btn_clicked();
    void discover_flash_disk();
    void export_completed(bool bok);
    void on_stack_changed(int);
};

#endif // QGENIEINSERTFLASHDISKPAGE_H
