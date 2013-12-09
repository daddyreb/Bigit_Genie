#ifndef BARCHART_H
#define BARCHART_H

#include <QWidget>
#include <QList>

class Chart : public QWidget
{
    Q_OBJECT

public:
    Chart(int titleid, int unitid, QWidget * parent=0);

protected:
    virtual void paintEvent(QPaintEvent * event)=0;
    virtual void changeEvent(QEvent* event);

public slots:
    virtual void updateData(QList<float> data) = 0;

protected:
    int titleid,unitid;
    QString title,unit;
    int xmargin,ymargin;

private:
    virtual void retranslateUi();
};

class BarChart : public Chart
{
    Q_OBJECT

public:
    BarChart(int titleidid, int unitid, QList<int>xtitleid,QWidget * parent=0);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void changeEvent(QEvent* event);

public slots:
    virtual void updateData(QList<float> data);

private:
    int spacewidth,barwidth,unitheight,unitwidth,xtitleheight;
    int colorflagrectwidth,colorflagtextwidth,colorflagtextheight;
    int xbarlength,ybarlength;
    int coordinatescalewidth,coordinatescaleheight;
    QList<QString> xtitle;
    QList<int> xtitleid;
    QList<float> data;
    QString uploadstr;
    QString downloadstr;

private:
    virtual void retranslateUi();
};

#endif // BARCHART_H
