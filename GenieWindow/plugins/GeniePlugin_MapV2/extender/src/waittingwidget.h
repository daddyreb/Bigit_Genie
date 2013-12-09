#ifndef WAITTINGWIDGET_H
#define WAITTINGWIDGET_H

#include <QWidget>
#include <QString>
#include "myprogressindicator.h"


class QEvent;
class QLabel;

class WaittingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaittingWidget(QWidget *parent = 0);
    ~WaittingWidget();

private:
    MyProgressIndicator  *Indicator;
    QLabel              *PromptLabel;

protected:
    virtual void changeEvent(QEvent *);

private:
    void retranslateUi();
    QString translateText(int idx, const QString &defaultText = QString());

signals:
    void translateText(int idx ,QString *text);

public slots:
    void start();
    void stop();
};

#endif // WAITTINGWIDGET_H
