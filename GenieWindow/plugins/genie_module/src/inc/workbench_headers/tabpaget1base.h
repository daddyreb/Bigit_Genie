#ifndef TABPAGET1BASE_H
#define TABPAGET1BASE_H

#include "tabpage.h"

class TabPageT1Base : public TabPage
{
	Q_OBJECT

public:
	explicit TabPageT1Base(QWidget *parent=0);
	~TabPageT1Base();
public:
	virtual TabPageType type(){return TabPage::Type1;}
private:
	
};

#endif // TABPAGET1BASE_H
