#ifndef ADVACEWIRELESSSETTINGPANE_H
#define ADVACEWIRELESSSETTINGPANE_H

#include <QWidget>

namespace Ui {
class AdvanceWirelessSettingPane;
}

class QEvent;

class AdvanceWirelessSettingPane : public QWidget
{
    Q_OBJECT
    
public:
    explicit AdvanceWirelessSettingPane(QWidget *parent = 0);
    ~AdvanceWirelessSettingPane();

public:
    void setAdvanceSettingInfo(const QString &model, const QString &ip);

protected:
    void changeEvent(QEvent *event);
private:
    void retranslateUi();

private slots:
    void onLinkActivated(const QString &url);

private:
    Ui::AdvanceWirelessSettingPane *ui;
    QString m_devModel;
    QString m_devIP;
};

#endif // ADVACEWIRELESSSETTINGPANE_H
