#ifndef QGENIEWIFIPROFILEMANAGERWIDGET_H
#define QGENIEWIFIPROFILEMANAGERWIDGET_H

#include <QWidget>
#include "QGenieWiFiCreateProfileWidget.h"
#include "QGenieWiFiProfileWidget.h"

class QGenieWiFiProfileManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGenieWiFiProfileManagerWidget(QWidget *parent = 0);

private:
    QGenieWiFiProfileWidget         *m_profileWidget;
    QGenieWiFiCreateProfileWidget   *m_newProfileWidget;
signals:
    void networkList();
private slots:
    void showCreateProfileWidget();
    void showProfileListWidget();
    void addProfileCompleted(bool added);
public:
    void beginManage();
};

#endif // QGENIEWIFIPROFILEMANAGERWIDGET_H
