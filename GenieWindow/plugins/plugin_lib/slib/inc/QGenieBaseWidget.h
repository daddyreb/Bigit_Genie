#ifndef QGENIEBASEWIDGET_H
#define QGENIEBASEWIDGET_H

#include <QWidget>

class QGenieBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGenieBaseWidget(QWidget *parent = 0);
protected:
    QString getI18nLan(int idx);
    void setBigButtonStyle(const QString & s);

};

#endif // QGENIEBASEWIDGET_H
