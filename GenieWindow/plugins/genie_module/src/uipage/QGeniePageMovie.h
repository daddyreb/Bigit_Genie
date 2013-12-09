#ifndef QGeniePageMovie_H
#define QGeniePageMovie_H

#ifndef IN_STABLE_H
#include <QObject>
#include <QGraphicsView>
#include <QTimeLine>
#endif


class QGeniePageMovie:public QGraphicsView
{
    Q_OBJECT
public:
    QGeniePageMovie(QWidget *parent=NULL);
    void setNext();
protected:
    QGraphicsScene *mScene;
    QGraphicsPixmapItem *mItemCurPage;
    QGraphicsPixmapItem *mItemNextPage;
    QTimeLine  *mTimeLine;



protected:

    void initPixmap();
    void showEvent ( QShowEvent * event ) ;
public slots:
    void slot_FrameUpdate(int val);
    void slot_Finished();
};


#endif // QGeniePageMovie_H
