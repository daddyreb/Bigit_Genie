#include "qLineEditPlaceTxt.h"

qLineEditPlaceTxt::qLineEditPlaceTxt(QWidget *parent) :
    QLineEdit(parent)
{
    strPlaceTxt="";
    connect (this,SIGNAL(editingFinished ()),this,SLOT(editingFinisheda ()));
}
/*
 void qLineEditPlaceTxt::mousePressEvent ( QMouseEvent * e )
 {
    if (this->text()==strPlaceTxt)
    {
       // this->setText("");
    }
 }
*/
 void qLineEditPlaceTxt::setPlaceTxt (QString txt)
 {
    strPlaceTxt=txt;
    //this->setText(strPlaceTxt);
 }

 void qLineEditPlaceTxt::editingFinisheda ()
 {
     if (this->text()=="")
     {
        // this->setText(strPlaceTxt);
     }
 }
