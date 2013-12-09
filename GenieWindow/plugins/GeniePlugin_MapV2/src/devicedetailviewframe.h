#ifndef DEVICEDETAILVIEWFRAME_H
#define DEVICEDETAILVIEWFRAME_H

#include <QWidget>
#include <QLabel>
#include <QPoint>
#include <QRect>

class QMouseEvent;
class QFrame;

class DeviceDetailViewFrame : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceDetailViewFrame(QWidget* content,QWidget *parent = 0);
public slots:
    void setFrameTitle(const QString &title);
    void setFrameLogo(const QPixmap &logo);
private:
    QLabel                          *m_logoLabel;
    QLabel                          *m_frameTitleLabel;
    QWidget                         *m_contentWidget;
    QFrame                          *m_frameContainer;

//    QPoint                          m_pos;
//    bool                            m_canMove;

    int                             m_frameTitleLabel_height;
    int                             m_frameTitleLabel_spaceitemw;

private:
    void prepareUi();
    void initFrame();
    void applyStyle();

//protected:
//    virtual void mouseMoveEvent(QMouseEvent *event);
//    virtual void mousePressEvent(QMouseEvent *event);
//    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    void centerInParent();
};

#endif // DEVICEDETAILVIEWFRAME_H
