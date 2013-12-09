#ifndef QGENIEHISTOGRAMWIDGET_H
#define QGENIEHISTOGRAMWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QEvent>
#include <QPoint>
#include <QStringList>
#include <QPushButton>

#include "QGenieColumnPanel.h"
#include "QGenieXLabelPanel.h"
#include "QGenieWirelessChannelLegendWidget.h"

class QGenieHistogramWidget : public QWidget
{

    Q_OBJECT
public:
    explicit QGenieHistogramWidget(QWidget *parent = 0);

protected:
    QGenieColumnPanel                   *m_column_panel;
    QGenieXLabelPanel                   *m_xlabel_panel;
    QGenieWirelessChannelLegendWidget   *m_legend_panel;
    QLabel                              *x_axis_label;
    QLabel                              *y_axis_label;
    QLabel                              *prompt_user_to_coulmn_tooltip;
    QWidget                             *prompt_legend_panel;
protected:
    //painting members
    QPoint                              x_axisleft_pt;
    QPoint                              x_axisright_pt;

    QPoint                              y_axistop_pt;
//    QPoint                              y_axisbottom_pt;

    QPoint                              column_panel_lefttop_pt;
    QPoint                              column_panel_rightbottom_pt;

    //label panel
    QPoint                              xlabel_panel_lefttop_pt;
    QPoint                              xlabel_panel_rightbottom_pt;

    //xy axis arrow
    QPoint                              x_axis_arrow_up_pt;
    QPoint                              x_axis_arrow_bottom_pt;

    QPoint                              y_axis_arrow_left_pt;
    QPoint                              y_axis_arrow_right_pt;

    //xy axis label
    QPoint                              x_axis_label_lefttop_pt;
    QPoint                              x_axis_label_rightbottom_pt;

    QPoint                              y_axis_label_lefttop_pt;
    QPoint                              y_axis_label_rightbottom_pt;

private:
    QPushButton                         *m_PrevPageBtn;
    QPushButton                         *m_NextPageBtn;
    QLabel                              *m_PagesLabel;

    void createPageBtnsAndLabel();
private:
    QList<unsigned int>                 m_dataList;
    QList<int>                          m_labelDataList;
    QStringList                         m_tooltipList;
private:
    int                                 m_pagesCount;
    int                                 m_curPageIndex;
    int                                 m_highlightPageIndex;
    int                                 m_highlightDataIndexInPage;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual QSize sizeHint() const;
    virtual void changeEvent(QEvent *event);
protected:
    void retranslateUi();
public:
    void loadData(const QList<unsigned int> &datalist,
                  const QList<int> &labeldatalist,
                  const QStringList &tooltiplist,
                  int highlightDataIndex);
private:
    void viewCurrentPage();
    void updateUiElements();
signals:
    void viewCurrentPage(int curPage);
private slots:
    void pageBtnClicked();
public:
    virtual bool eventFilter(QObject *obj, QEvent *event);
};

#endif // QGENIEHISTOGRAMWIDGET_H
