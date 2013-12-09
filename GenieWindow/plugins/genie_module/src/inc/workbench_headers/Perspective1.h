#ifndef PERSPECTIVE1_H
#define PERSPECTIVE1_H
#include "perspective.h"

class Perspective1 :
	public Perspective
{
public:
	Perspective1 (Qt::Orientation orientation, QWidget * parent = 0);
	~Perspective1(void);
};

#endif