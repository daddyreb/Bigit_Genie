#ifndef PERSPECTIVE2_H
#define PERSPECTIVE2_H

#include "perspective.h"

class Perspective2 :
	public Perspective
{
public:

	Perspective2 (Qt::Orientation orientation, QWidget * parent = 0);
	~Perspective2(void);
};

#endif