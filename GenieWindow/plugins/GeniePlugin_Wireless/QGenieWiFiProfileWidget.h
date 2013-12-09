#ifndef QGENIEWIFIPROFILEWIDGET_H
#define QGENIEWIFIPROFILEWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
    class QGenieWiFiProfileWidget;
}

class QGenieWiFiProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QGenieWiFiProfileWidget(QWidget *parent = 0);
    ~QGenieWiFiProfileWidget();

private:
    Ui::QGenieWiFiProfileWidget *ui;
    int                         m_selectedRow;
public:
    void addProfileCompleted(bool added);
private:
    void deleteProfile();
    void moveUpProfile();
    void moveDownProfile();

    void updateUiElements();

    void exchangeProfileListItem(int row1,int row2);
signals:
    void requestAddProfile();
    void networkList();
    void error();
private slots:
    void btnClicked();
    void itemSelectionChanged();
    bool reloadData();
public:
    void init();
protected:
    virtual void changeEvent(QEvent *event);
private:
    void retranslateUi();
};

#endif // QGENIEWIFIPROFILEWIDGET_H
