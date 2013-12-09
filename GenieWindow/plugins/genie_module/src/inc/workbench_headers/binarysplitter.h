#ifndef BINARYSPLITTER_H
#define BINARYSPLITTER_H

#include <QSplitter>

class BinarySplitter : public QSplitter
{
	Q_OBJECT
public:
	explicit BinarySplitter(QWidget *parent);
	BinarySplitter (Qt::Orientation orientation, QWidget * parent = 0);
	~BinarySplitter();
public:
	void setCollapsible (int index, bool collapse);
	void setChildrenCollapsible(bool collapse);
	void setHandleWidth(int width);
public:
	void setLeftNode(QWidget* left);
	void setRightNode(QWidget* right);

	QWidget* leftNode();
	QWidget* rightNode();

	bool isLeftNode(QWidget* widget);

	void setLeftStretchFactor(int stretch);
	void setRightStretchFactor(int stretch);

};

#endif // BINARYSPLITTER_H
