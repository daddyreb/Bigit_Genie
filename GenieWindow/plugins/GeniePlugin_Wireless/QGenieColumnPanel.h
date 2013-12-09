#ifndef QGENIECOLUMNPANEL_H
#define QGENIECOLUMNPANEL_H

#include <QWidget>
#include <QStringList>
#include <QList>

#include "QGenieColumnUnit.h"

class QGenieColumnPanel : public QWidget
{
Q_OBJECT
public:
    explicit QGenieColumnPanel(QWidget *parent = 0);
private:
    int                         m_twopartsblock_index;
    QList<unsigned int>         m_columndata_list;
    QStringList                 m_tooltips_list;

    //ui
    QList<QGenieColumnUnit*>    m_columnunit_list;

public:
    void loadData(const QList<unsigned int> &blockDataList, const QStringList &blockTipsList, int twoPartsBlockIndex = -1);
private:
    void updateColumnUnits();
    void updateUi();
protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // QGENIECOLUMNPANEL_H
