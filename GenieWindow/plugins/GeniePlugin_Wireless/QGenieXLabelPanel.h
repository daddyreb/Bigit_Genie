#ifndef QGENIEXLABELPANEL_H
#define QGENIEXLABELPANEL_H

#include <QWidget>
#include <QLabel>
#include <QList>

class QGenieXLabelPanel : public QWidget
{
Q_OBJECT
public:
    explicit QGenieXLabelPanel(QWidget *parent = 0);
private:
    QList<QLabel *> m_label_list;
public:
    void loadData(const QList<int> &labelDataList);
};

#endif // QGENIEXLABELPANEL_H
