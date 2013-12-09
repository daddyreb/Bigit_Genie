#ifndef QGENIEFRAMELEFTLAYOUT_H
#define QGENIEFRAMELEFTLAYOUT_H
#ifndef IN_STABLE_H
#include <QObject>
#include "ui_QGenieFrameLeftLayout.h"
#endif



class QPushButton;
class QGenieFrameLeftLayout:public QFrame
{
       Q_OBJECT
public:
    QGenieFrameLeftLayout(QWidget *parent=NULL);
  //  QGenieFrameLeftLayout();
    void addButton(QPushButton *bt);
    void deleteButton(QPushButton *bt);
    void checkButton(int idx); //checked the button when big button clicked
    inline int getCount()
    {
      return mButNum;
    };

protected:
    void changeEvent(QEvent* event);

private:
    Ui::QGenieFrameLeftLayout mUi;
    static QButtonGroup mButGroup;
    int mButNum;
};

#endif // QGenieFrameLeftLayout_H
