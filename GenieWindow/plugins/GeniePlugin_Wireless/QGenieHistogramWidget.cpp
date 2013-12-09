#include "QGenieHistogramWidget.h"
#include"QGenieWirelessCommon.h"
#include "GeniePlugin_Wireless.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QLinearGradient>
#include <assert.h>
#include "genie2_interface.h"

void setButtonIco(QPushButton *but,const QString &ico)
{
    Q_ASSERT(but);
#ifdef Q_OS_WIN32
    but->setIcon(QIcon(ico));
#else
    but->setStyleSheet(QString("border-image: url(%1)").arg(ico));
#endif
}

QGenieHistogramWidget::QGenieHistogramWidget(QWidget *parent) :
        QWidget(parent),
        m_pagesCount(0),
        m_curPageIndex(-1),
        m_highlightPageIndex(-1),
        m_highlightDataIndexInPage(-1)


{
    QFont label_font;
    label_font.setBold(true);
    x_axis_label                    = new QLabel(this);
    y_axis_label                    = new QLabel(this);

    x_axis_label->setFont(label_font);
    y_axis_label->setFont(label_font);

    x_axis_label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    y_axis_label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    y_axis_label->setWordWrap(true);

    prompt_user_to_coulmn_tooltip   = new QLabel(this);
    prompt_user_to_coulmn_tooltip->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    prompt_user_to_coulmn_tooltip->setAlignment(Qt::AlignCenter);
    prompt_user_to_coulmn_tooltip->setWordWrap(true);

    m_column_panel = new QGenieColumnPanel(this);
    m_xlabel_panel = new QGenieXLabelPanel(this);
    m_legend_panel = new QGenieWirelessChannelLegendWidget(this);
    m_legend_panel->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    prompt_legend_panel = new QWidget(this);
    QHBoxLayout *pl_panel_layout = new QHBoxLayout(prompt_legend_panel);
    pl_panel_layout->setContentsMargins(0,0,0,0);
    pl_panel_layout->addWidget(prompt_user_to_coulmn_tooltip);
    pl_panel_layout->addWidget(m_legend_panel);
    pl_panel_layout->setAlignment(m_legend_panel,Qt::AlignRight);
    prompt_legend_panel->setLayout(pl_panel_layout);

    createPageBtnsAndLabel();

    retranslateUi();
}

void QGenieHistogramWidget::createPageBtnsAndLabel()
{
    m_PrevPageBtn = new QPushButton(this);
    m_NextPageBtn = new QPushButton(this);
    m_PagesLabel  = new QLabel(this);

    m_PagesLabel->setAlignment(Qt::AlignCenter);

    m_PrevPageBtn->setFocusPolicy(Qt::NoFocus);
    m_NextPageBtn->setFocusPolicy(Qt::NoFocus);

//    m_PrevPageBtn->setIcon(QIcon());™
    setButtonIco(m_PrevPageBtn,":/image/LBt.png");
    m_PrevPageBtn->setFixedSize(18,18);
    m_PrevPageBtn->setFlat(true);

//    m_NextPageBtn->setIcon(QIcon());
    setButtonIco(m_NextPageBtn,":/image/RBt.png");
    m_NextPageBtn->setFixedSize(18,18);
    m_NextPageBtn->setFlat(true);

    connect(m_PrevPageBtn,SIGNAL(clicked()),this,SLOT(pageBtnClicked()));
    connect(m_NextPageBtn,SIGNAL(clicked()),this,SLOT(pageBtnClicked()));

    m_PrevPageBtn->installEventFilter(this);
    m_NextPageBtn->installEventFilter(this);

}

void QGenieHistogramWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLinearGradient lg(this->rect().left(),this->rect().top(),this->rect().right(),this->rect().bottom());
    lg.setColorAt(0,QColor(236,236,236));
    lg.setColorAt(1,QColor(236,236,236));
    painter.fillRect(rect(),lg);

    if(m_curPageIndex != -1)
    {
        painter.setRenderHint(QPainter::Antialiasing);
        QPen pen(Qt::blue);
        pen.setWidth(X_AXIS_LINE_WIDTH);
        pen.setCapStyle(Qt::SquareCap);
        pen.setJoinStyle(Qt::MiterJoin);

        painter.setPen(pen);
        painter.drawLine(x_axisleft_pt,x_axisright_pt);

        /*QPoint temp_pt;
        //draw arrow
        temp_pt = x_axisright_pt;
        temp_pt.setX(temp_pt.x()+X_AXIS_LINE_WIDTH);
        painter.drawLine(x_axis_arrow_up_pt,temp_pt);
        painter.drawLine(x_axis_arrow_bottom_pt,temp_pt);*/

        pen.setWidth(Y_AXIS_LINE_WIDTH);
        painter.setPen(pen);
        painter.drawLine(y_axistop_pt,x_axisleft_pt);
        //draw arrow
        /*temp_pt = y_axistop_pt;
        temp_pt.setY(temp_pt.y()-Y_AXIS_LINE_WIDTH);
        painter.drawLine(temp_pt,y_axis_arrow_left_pt);
        painter.drawLine(temp_pt,y_axis_arrow_right_pt);*/
    }
}

void QGenieHistogramWidget::showEvent(QShowEvent *event)
{
    updateUiElements();
    QWidget::showEvent(event);
}

void QGenieHistogramWidget::resizeEvent(QResizeEvent *event)
{
    QSize new_size = event->size();

    x_axisleft_pt.setX(Y_AXIS_LEFT_MARGIN);
    x_axisleft_pt.setY(new_size.height()-1-X_AXIS_BOTTOM_MARGIN);

    x_axisright_pt.setX(new_size.width()-1-X_AXIS_RIGHT_MARGIN);
    x_axisright_pt.setY(x_axisleft_pt.y());

    y_axistop_pt.setX(x_axisleft_pt.x());
    y_axistop_pt.setY(Y_AXIS_UP_MARGIN);

    column_panel_lefttop_pt.setX(y_axistop_pt.x()+
                                 Y_AXIS_COLUMNPANEL_LEFT_SPACE+Y_AXIS_LINE_WIDTH);
    column_panel_lefttop_pt.setY(y_axistop_pt.y()+Y_AXIS_UP_COLUMNPANEL_TOP_SPACE);

    column_panel_rightbottom_pt.setX(x_axisright_pt.x()-X_AXIS_RIGHT_COLUMNPANEL_RIGHT_SPACE);
    column_panel_rightbottom_pt.setY(x_axisleft_pt.y()-X_AXIS_LINE_WIDTH);


    xlabel_panel_lefttop_pt.setX(column_panel_lefttop_pt.x());
    xlabel_panel_lefttop_pt.setY(x_axisleft_pt.y()+X_AXIS_LINE_WIDTH);

    xlabel_panel_rightbottom_pt.setX(column_panel_rightbottom_pt.x());
    xlabel_panel_rightbottom_pt.setY(new_size.height()-1);

    x_axis_arrow_up_pt.setX(x_axisright_pt.x()-XY_AXISARROW_HEIGHT+1);
    x_axis_arrow_up_pt.setY(x_axisright_pt.y()-XY_AXISARROW_WIDTH);

    x_axis_arrow_bottom_pt.setX(x_axis_arrow_up_pt.x());
    x_axis_arrow_bottom_pt.setY(x_axisright_pt.y()+XY_AXISARROW_WIDTH);

    y_axis_arrow_left_pt.setX(y_axistop_pt.x()-XY_AXISARROW_WIDTH);
    y_axis_arrow_left_pt.setY(y_axistop_pt.y()+XY_AXISARROW_HEIGHT-1);

    y_axis_arrow_right_pt.setX(y_axistop_pt.x()+XY_AXISARROW_WIDTH);
    y_axis_arrow_right_pt.setY(y_axis_arrow_left_pt.y());


    x_axis_label_lefttop_pt.setX(column_panel_rightbottom_pt.x()+1);
    x_axis_label_lefttop_pt.setY(x_axisright_pt.y()+XY_AXISARROW_WIDTH);

    x_axis_label_rightbottom_pt.setX(x_axis_label_lefttop_pt.x()+X_AXISLABEL_WIDTH);
    x_axis_label_rightbottom_pt.setY(x_axis_label_lefttop_pt.y()+Y_AXISLABEL_HEIGHT);

    y_axis_label_lefttop_pt.setX(0);
    y_axis_label_lefttop_pt.setY(y_axistop_pt.y());

    y_axis_label_rightbottom_pt.setX(y_axistop_pt.x()-XY_AXISARROW_WIDTH-Y_AXIS_LINE_WIDTH/*-2*/);
    y_axis_label_rightbottom_pt.setY(y_axistop_pt.y()+Y_AXISLABEL_HEIGHT);

    QRect column_panel_rect(column_panel_lefttop_pt,column_panel_rightbottom_pt);
    m_column_panel->setGeometry(column_panel_rect);

    QRect xlabel_panel_rect(xlabel_panel_lefttop_pt,xlabel_panel_rightbottom_pt);
    m_xlabel_panel->setGeometry(xlabel_panel_rect);

    prompt_legend_panel->move(y_axistop_pt+QPoint(2,-Y_AXIS_UP_MARGIN));
    prompt_legend_panel->resize(this->size().width()-y_axistop_pt.x()-2,prompt_legend_panel->size().height());

    QRect x_axis_label_rect(x_axis_label_lefttop_pt,x_axis_label_rightbottom_pt);
    x_axis_label->setGeometry(x_axis_label_rect);
    QRect y_axis_label_rect(y_axis_label_lefttop_pt,y_axis_label_rightbottom_pt);
    y_axis_label->setGeometry(y_axis_label_rect);

    //layout page buttons
    m_PrevPageBtn->move(2,x_axisleft_pt.y() + 10);
    m_NextPageBtn->move(Y_AXIS_LEFT_MARGIN - 20,x_axisleft_pt.y() + 10);
    m_PagesLabel->move(20,x_axisleft_pt.y() + 10);
    m_PagesLabel->setFixedWidth(Y_AXIS_LEFT_MARGIN - 42);

    QWidget::resizeEvent(event);
}


QSize QGenieHistogramWidget::sizeHint() const
{
    return QSize(Y_AXIS_LEFT_MARGIN + Y_AXIS_LINE_WIDTH + Y_AXIS_COLUMNPANEL_LEFT_SPACE
                 +m_xlabel_panel->sizeHint().width() + X_AXIS_RIGHT_COLUMNPANEL_RIGHT_SPACE +
                 X_AXIS_RIGHT_MARGIN + 100,
                 Y_AXIS_UP_MARGIN + m_column_panel->sizeHint().height() + X_AXIS_LINE_WIDTH +
                 m_xlabel_panel->sizeHint().height() + 120 );
}

void QGenieHistogramWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QWidget::changeEvent(event);
}

void QGenieHistogramWidget::retranslateUi()
{
    x_axis_label->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_HW_X_AXIS_LB_TEXT));
    y_axis_label->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_HW_Y_AXIS_LB_TEXT));

    prompt_user_to_coulmn_tooltip->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_WCV_WIFI_PROMPT_USER_TO_TOOLTIP_TEXT));
}

void QGenieHistogramWidget::loadData(const QList<unsigned int> &datalist,
                                     const QList<int> &labeldatalist,
                                     const QStringList &tooltiplist,
                                     int highlightDataIndex)
{
    m_pagesCount = (datalist.count() == 0?0:1);
    m_highlightPageIndex = (highlightDataIndex == -1?-1:0);


    if(datalist.count() > CHANNELCOUNT_IN_2DOT4GHZFREQUENCY)
    {
        m_pagesCount += ((datalist.count() - CHANNELCOUNT_IN_2DOT4GHZFREQUENCY - 1) / CHANNELCOUNT_2VIEWPERPAGE + 1);

        if(highlightDataIndex >= CHANNELCOUNT_IN_2DOT4GHZFREQUENCY)
        {
            m_highlightPageIndex += ((highlightDataIndex - CHANNELCOUNT_IN_2DOT4GHZFREQUENCY) / CHANNELCOUNT_2VIEWPERPAGE + 1);
        }
    }

    m_curPageIndex = ((m_pagesCount > 0?(m_highlightPageIndex == -1?0:m_highlightPageIndex):-1));

    if(m_highlightPageIndex <= -1)
    {
        m_highlightDataIndexInPage  = -1;
    }
    else if(m_highlightPageIndex == 0)
    {
        m_highlightDataIndexInPage = highlightDataIndex;
    }
    else if(m_highlightPageIndex > 0)
    {
        m_highlightDataIndexInPage = ((highlightDataIndex - CHANNELCOUNT_IN_2DOT4GHZFREQUENCY) % CHANNELCOUNT_2VIEWPERPAGE);
    }


    m_dataList = datalist;
    m_tooltipList = tooltiplist;
    m_labelDataList = labeldatalist;

    updateUiElements();
    viewCurrentPage();
}

void QGenieHistogramWidget::updateUiElements()
{
    m_column_panel->setVisible(m_curPageIndex != -1);
    m_xlabel_panel->setVisible(m_curPageIndex != -1);

    m_PagesLabel->setVisible(m_curPageIndex != -1);
    prompt_legend_panel->setVisible(m_curPageIndex != -1);
    x_axis_label->setVisible(m_curPageIndex != -1);
    y_axis_label->setVisible(m_curPageIndex != -1);

    m_PrevPageBtn->setVisible((m_curPageIndex != -1) && (m_pagesCount > 1));
    m_NextPageBtn->setVisible((m_curPageIndex != -1) && (m_pagesCount > 1));
    m_PagesLabel->setVisible((m_curPageIndex != -1) && (m_pagesCount > 1));

    m_PrevPageBtn->setEnabled(m_curPageIndex > 0);
    m_NextPageBtn->setEnabled((m_curPageIndex != -1) && (m_curPageIndex < (m_pagesCount - 1)));

    update();
}

void QGenieHistogramWidget::viewCurrentPage()
{
    int pos = 0, length = -1;

    if(m_curPageIndex == 0)
    {
        pos = 0;
        length = CHANNELCOUNT_IN_2DOT4GHZFREQUENCY;
    }
    else if(m_curPageIndex > 0)
    {
        pos += (CHANNELCOUNT_IN_2DOT4GHZFREQUENCY + (m_curPageIndex - 1) * CHANNELCOUNT_2VIEWPERPAGE);
        length = ((m_dataList.count() - pos) > CHANNELCOUNT_2VIEWPERPAGE?CHANNELCOUNT_2VIEWPERPAGE:-1);
    }

    if(m_curPageIndex != -1)
    {
        m_column_panel->loadData(m_dataList.mid(pos,length),
                                 m_tooltipList.mid(pos,length),
                                 (m_curPageIndex == m_highlightPageIndex)?m_highlightDataIndexInPage:-1);
        m_xlabel_panel->loadData(m_labelDataList.mid(pos,length));

        m_PagesLabel->setText(QString("%1").arg(m_curPageIndex + 1) + "/" + QString("%1").arg(m_pagesCount));
    }
    else
    {
        m_column_panel->loadData(m_dataList,m_tooltipList);
        m_xlabel_panel->loadData(m_labelDataList);
        m_PagesLabel->setText("");
    }

    emit viewCurrentPage(m_curPageIndex);
}

void QGenieHistogramWidget::pageBtnClicked()
{
    QPushButton *sender = qobject_cast<QPushButton *>(this->sender());
    if(sender == m_PrevPageBtn)
    {
        if(m_curPageIndex > 0)
        {
            --m_curPageIndex;
            updateUiElements();
            viewCurrentPage();
        }
    }
    else if(sender == m_NextPageBtn)
    {
        if(m_curPageIndex < (m_pagesCount - 1))
        {
            ++m_curPageIndex;
            updateUiElements();
            viewCurrentPage();
        }
    }
}

bool QGenieHistogramWidget::eventFilter(QObject *obj, QEvent *event)
 {
     if ((obj == m_PrevPageBtn) || (obj == m_NextPageBtn))
    {
         QPushButton *btn = qobject_cast<QPushButton *>(obj);

         if (event->type() == QEvent::EnabledChange)
         {
             setButtonIco(btn,
                          btn == m_PrevPageBtn?
                             GENIE2_RES (btn->isEnabled()?"map/others/Prev.png":"map/others/PrevDisabled.png")
                            :GENIE2_RES(btn->isEnabled()?"map/others/Next.png":"map/others/NextDisabled.png"));
//             btn->setIcon((btn == m_PrevPageBtn?(btn->isEnabled()?QIcon(":/image/LBt.png"):QIcon(":/image/LBtDisable.png")):(btn->isEnabled()?QIcon(":/image/RBt.png"):QIcon(":/image/RBtDisable.png"))));
         }
         else if(event->type() == QEvent::Enter)
         {
             if(btn->isEnabled()){
                 setButtonIco(btn,GENIE2_RES(btn == m_PrevPageBtn?"map/others/PrevSelected.png" : "map/others/NextSelected.png"));
//                 btn->setIcon(btn == m_PrevPageBtn?QIcon(":/image/LBtSelect.png"):QIcon(":/image/RBtSelect.png"));
             }
         }
         else if(event->type() == QEvent::Leave)
         {
             if(btn->isEnabled()){
                 setButtonIco(btn,GENIE2_RES(btn == m_PrevPageBtn?"map/others/Prev.png" : "map/others/Next.png"));
//                 btn->setIcon(btn == m_PrevPageBtn?QIcon(":/image/LBt.png"):QIcon(":/image/RBt.png"));
             }
         }
         else
         {
             return false;
         }

         return true;
     }
     else
     {
         return QWidget::eventFilter(obj, event);
     }
 }
