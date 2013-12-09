#ifndef WIRELESSSETTINGOPERATEPANE_H
#define WIRELESSSETTINGOPERATEPANE_H

#include <QWidget>

namespace Ui {
class WirelessSettingOperatePane;
}

class QEvent;

class WirelessSettingOperatePane : public QWidget
{
    Q_OBJECT
    
public:
    explicit WirelessSettingOperatePane(QWidget *parent = 0);
    ~WirelessSettingOperatePane();

public:
    void enableApply(bool enable);

signals:
    void applyClicked();
    void cancelClicked();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private:
    Ui::WirelessSettingOperatePane *ui;
};

#endif // WIRELESSSETTINGOPERATEPANE_H
