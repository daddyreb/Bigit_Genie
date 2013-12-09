#ifndef QGENIEWATERWINDOW_H
#define QGENIEWATERWINDOW_H

#define STATIC_TITLE 75
#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#endif

class QGenieAboutThread;

class QGenieWaterWindow : public QWidget
{
	Q_OBJECT
public:
    QGenieWaterWindow(QWidget *parent = NULL);
    ~QGenieWaterWindow();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent*);
    void hideEvent ( QHideEvent * event );
    void showEvent ( QShowEvent * event )   ;

private:
    void calcWater(int npage, int density);

    void addBlob(int x, int y, int radius, int height);

    void drawWater(QRgb* srcImage, QRgb* dstImage);

    static QRgb shiftColor(QRgb color,int shift) {
        return qRgb(qBound(0, qRed(color) - shift,  255),
            qBound(0, qGreen(color) - shift,255),
            qBound(0, qBlue(color) - shift, 255));
    }



 //   QImage          image;
 //   QImage          image1;
    QGenieAboutThread          *m_thread;
protected slots:
    void slot_SetSize(const QSize &siz );


};

#endif // WATERWINDOW_H
