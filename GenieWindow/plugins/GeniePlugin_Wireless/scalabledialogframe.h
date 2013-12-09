#ifndef SCALABLEDIALOGFRAME_H
#define SCALABLEDIALOGFRAME_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QList>

class ScalableDialogFrame;
class ScalableDialogContentWidget;

class ScalableDialogContentWidget : public QWidget
{
    friend class ScalableDialogFrame;
Q_OBJECT
protected:
    explicit ScalableDialogContentWidget(QWidget *parent = 0);
    virtual ~ScalableDialogContentWidget();
protected:
    ScalableDialogFrame     *m_container_dlg;
};


class ScalableDialogFrame : public QDialog
{
    friend class ScalableDialogContentWidget;
Q_OBJECT
private:
    explicit ScalableDialogFrame(ScalableDialogContentWidget* content,QWidget *parent = 0);
public:
    void setFrameTitle(const QString &title);
private:
    QLabel                                  *m_dlgtitle_lb;
    ScalableDialogContentWidget             *m_content_widget;

    QPoint pos0;
    QPoint last;
    bool   m_bDrag;

    int                             m_dlg_width;
    int                             m_dlg_height;
    int                             m_dlgtitle_lb_height;
    int                             m_dlgtitle_lb_spaceitemw;
    int                             m_frame_lr_margin;
    int                             m_frame_tb_margin;

private:
    void init_dim();
    void init_frame();
    void init_container();
    void applystyle();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // SCALABLEDIALOGFRAME_H
