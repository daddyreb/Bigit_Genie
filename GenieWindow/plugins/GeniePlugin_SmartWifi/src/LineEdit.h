/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class QToolButton;

class LineEdit : public QLineEdit
{
	//�������ť��QLineEdit
    Q_OBJECT

public:
    LineEdit(QWidget *parent = 0);
	virtual ~LineEdit();
	void SetCompliterStringList(QStringList& list);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateCloseButton(const QString &text);
private:
    QToolButton *clearButton;

};

#endif // LIENEDIT_H