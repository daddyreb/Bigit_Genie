#include "QGenieColumnPanel.h"
#include "GeniePlugin_Wireless.h"
#include "QGenieWirelessCommon.h"
#include <QHBoxLayout>

QGenieColumnPanel::QGenieColumnPanel(QWidget *parent) :
    QWidget(parent),
    m_twopartsblock_index(-1)
{
}

void QGenieColumnPanel::resizeEvent(QResizeEvent *event)
{
    updateUi();
    QWidget::resizeEvent(event);
}

void QGenieColumnPanel::loadData(const QList<unsigned int> &blockDataList, const QStringList &blockTipsList, int twoPartsBlockIndex)
{
    m_columndata_list           = blockDataList;
    m_twopartsblock_index       = twoPartsBlockIndex;
    m_tooltips_list             = blockTipsList;

    updateUi();
}

void QGenieColumnPanel::updateColumnUnits()
{
    QGenieColumnUnit *coumn_unit    = NULL;
    QHBoxLayout *columns_layout     = qobject_cast<QHBoxLayout *>(layout());

    if(!columns_layout)
    {
         columns_layout = new QHBoxLayout(this);
         columns_layout->setMargin(0);
    }

    int delta = m_columndata_list.count() - m_columnunit_list.count();

    if(delta > 0)
    {
        for(int k = 0;k < delta;++k)
        {
            coumn_unit = new QGenieColumnUnit(this);
            columns_layout->addWidget(coumn_unit);
            columns_layout->setAlignment(coumn_unit,Qt::AlignHCenter);
            coumn_unit->show();

            m_columnunit_list.append(coumn_unit);
        }
    }
    else if(delta < 0)
    {
        for(int j = 0; j < -delta;++j)
        {
            coumn_unit = m_columnunit_list.takeLast();
            columns_layout->removeWidget(coumn_unit);
            delete coumn_unit;
        }
    }
    //delta == 0 nothing to do
}

void QGenieColumnPanel::updateUi()
{
    updateColumnUnits();

    if(m_columndata_list.count() > 0)
    {
        //compute the block height factors
        unsigned int maxData = m_columndata_list[0];
        for(int i = 1;i < m_columndata_list.count();++i)
        {
            if(maxData < m_columndata_list[i])
                maxData = m_columndata_list[i];
        }

        int column_block_height = m_columnunit_list[0]->getColumnBlockHeight();
        int column_quantity_height = 0;

        for(int k = 0;k < m_columndata_list.count();++k)
        {
            if(maxData > 0)
            {
                column_quantity_height = int(column_block_height*(float(m_columndata_list[k])/float(maxData)));
            }
            else
            {
                column_quantity_height = COLUMN_BLOCK_MINIMUMHEIGHT;
            }

            m_columnunit_list[k]->setColumnBlockHeight(column_quantity_height);

            if(column_quantity_height <= COLUMN_BLOCK_MINIMUMHEIGHT)
            {
                m_columnunit_list[k]->setLabelText(QString());
            }
            else
            {
                m_columnunit_list[k]->setLabelText(QString(tr("%1")).arg(m_columndata_list[k]));
            }

            m_columnunit_list[k]->updateColumnBlock((m_twopartsblock_index == k),m_columndata_list[k]);

            m_columnunit_list[k]->setColumnTooltip((k < m_tooltips_list.count()?m_tooltips_list[k]:QString()));
        }
    }
}
