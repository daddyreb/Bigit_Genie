#ifndef QLINEEDITPLACETXT_H
#define QLINEEDITPLACETXT_H

#include <QWidget>
#include <QLineEdit>
#include <QString>

class qLineEditPlaceTxt : public QLineEdit
{
Q_OBJECT
public:
    explicit qLineEditPlaceTxt(QWidget *parent = 0);


 //   void mousePressEvent ( QMouseEvent * e );
    void setPlaceTxt (QString txt);
    QString strPlaceTxt;
signals:

public slots:
    void editingFinisheda ();
};

#endif // QLINEEDITPLACETXT_H
