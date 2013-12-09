#include "QGenieXLabelPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

QGenieXLabelPanel::QGenieXLabelPanel(QWidget *parent) :
    QWidget(parent)
{
}

void QGenieXLabelPanel::loadData(const QList<int> &labelDataList)
{
    QHBoxLayout  *xlpanel_layout = qobject_cast<QHBoxLayout *>(layout());
    QLabel       *label          = NULL;

    if(!xlpanel_layout)
    {
        xlpanel_layout = new QHBoxLayout(this);
        xlpanel_layout->setMargin(0);
    }

    int delta = labelDataList.count() - m_label_list.count();

    if(delta > 0)
    {
        for(int i = 0;i < delta;++i)
        {
            label = new QLabel(this);
            label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
            xlpanel_layout->addWidget(label);
            xlpanel_layout->setAlignment(label,Qt::AlignHCenter);
            m_label_list.append(label);
        }
    }
    else if(delta < 0)
    {
        for(int k = 0;k < -delta;++k)
        {
            label = m_label_list.takeLast();
            xlpanel_layout->removeWidget(label);
            delete label;
        }
    }

    for(int j = 0;j < labelDataList.count();++j)
    {
        m_label_list[j]->setText(QString("%1").arg(labelDataList[j]));
    }
}
