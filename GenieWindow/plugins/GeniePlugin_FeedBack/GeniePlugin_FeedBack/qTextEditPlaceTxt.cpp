#include "qTextEditPlaceTxt.h"

qTextEditPlaceTxt::qTextEditPlaceTxt(QWidget *parent) :
    QTextEdit(parent)
{
    strPlaceTxt="";
    connect (this,SIGNAL(editingFinished ()),this,SLOT(editingFinisheda ()));
}
/*
 void qTextEditPlaceTxt::mousePressEvent ( QMouseEvent * e )
 {

    //textEdit->toPlainText()
    if (this->toPlainText()==strPlaceTxt)
    {
       // this->setPlainText("");
    }
 }
*/
 void qTextEditPlaceTxt::setPlaceTxt (QString txt)
 {
    strPlaceTxt=txt;
   // this->setPlainText(strPlaceTxt);
 }
 void	qTextEditPlaceTxt::focusOutEvent ( QFocusEvent * e )
 {
     if (this->toPlainText()=="")
     {
       //  this->setPlainText(strPlaceTxt);
     }
 }

 void qTextEditPlaceTxt::editingFinisheda ()
 {

 }
