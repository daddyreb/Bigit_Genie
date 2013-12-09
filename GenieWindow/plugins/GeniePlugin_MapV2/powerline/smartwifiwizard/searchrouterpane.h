#ifndef SEARCHROUTERPANE_H
#define SEARCHROUTERPANE_H

#include <QWidget>

namespace Ui {
class SearchRouterPane;
}

class QEvent;

class SearchRouterPane : public QWidget
{
    Q_OBJECT
    
public:
    explicit SearchRouterPane(QWidget *parent = 0);
    ~SearchRouterPane();

public:
    void ControlSearchRouterUi(bool startOrStop);
    void SetDeviceModel(const QString &model);
signals:
    void searchRouterClicked();

protected:
    void changeEvent(QEvent *);

private:
    void retranslateUi();

private:
    Ui::SearchRouterPane *ui;
    QString m_devModel;
};

#endif // SEARCHROUTERPANE_H
