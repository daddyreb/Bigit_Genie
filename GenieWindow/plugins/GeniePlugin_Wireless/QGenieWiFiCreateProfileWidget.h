#ifndef QGENIEWIFICREATEPROFILEWIDGET_H
#define QGENIEWIFICREATEPROFILEWIDGET_H

#include <QWidget>
#include <QEvent>

namespace Ui {
    class QGenieWiFiCreateProfileWidget;
}

class QGenieWiFiCreateProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QGenieWiFiCreateProfileWidget(QWidget *parent = 0);
    ~QGenieWiFiCreateProfileWidget();

private:
    Ui::QGenieWiFiCreateProfileWidget *ui;
signals:
    void addProfileCompleted(bool added);
private:
#ifdef Q_OS_WIN
    void initAuthList();
#elif defined Q_OS_MACX
    void initSecurityList();
#endif
    void updateUiElements();
    void createProfile();
    bool isNetworkKeyValid();
    bool isSsidValid();
private slots:
    void btnClicked();
    void currentIndexChanged(int index);
    void textChanged(const QString &text);
    void stateChanged(int state);
public:
    void resetUi();
protected:
    virtual void changeEvent(QEvent *event);
private:
    void retranslateUi();
};

#endif // QGENIEWIFICREATEPROFILEWIDGET_H
