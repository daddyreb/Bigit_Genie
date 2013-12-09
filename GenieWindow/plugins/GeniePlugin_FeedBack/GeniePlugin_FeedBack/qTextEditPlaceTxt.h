#ifndef QTEXTEDITPLACETXT_H
#define QTEXTEDITPLACETXT_H

#include <QWidget>
#include <QTextEdit>

class qTextEditPlaceTxt : public QTextEdit
{
Q_OBJECT
public:
    explicit qTextEditPlaceTxt(QWidget *parent = 0);

   // void mousePressEvent ( QMouseEvent * e );
    void setPlaceTxt (QString txt);
    QString strPlaceTxt;
    void focusOutEvent ( QFocusEvent * e );
signals:

public slots:
    void editingFinisheda ();

};

#endif // QTEXTEDITPLACETXT_H
