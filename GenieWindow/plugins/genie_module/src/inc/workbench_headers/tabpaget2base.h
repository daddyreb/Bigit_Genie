#ifndef TABPAGET2BASE_H
#define TABPAGET2BASE_H
#include "tabpage.h"

class TabPageT2Base : public TabPage
{
	Q_OBJECT

public:
	explicit TabPageT2Base(QWidget *parent=0);
	~TabPageT2Base();
public:
	virtual TabPageType type(){return TabPage::Type2;}
private:
	
};

#endif // TABPAGET2BASE_H
