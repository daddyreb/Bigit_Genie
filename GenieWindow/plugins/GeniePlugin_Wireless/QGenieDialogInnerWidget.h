#ifndef QGENIEDIALOGINNERWIDGET_H
#define QGENIEDIALOGINNERWIDGET_H

#include <QWidget>
#include <QDialog>

class QGenieDialogInnerWidget : public QWidget
{
Q_OBJECT
public:
    explicit QGenieDialogInnerWidget(QWidget *parent = 0);
protected:
    QDialog     *m_container_dlg;
public:
   void  set_container_dlg(QDialog *dlg);
   void  accept();
   void  reject();

   virtual QString get_dlg_title(){return QString();}
signals:

public slots:

};

#endif // QGENIEDIALOGINNERWIDGET_H
