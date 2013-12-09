#include "stable.h"
#ifdef USE_MAIN_PROXY
QGenieFrameMainProxy::QGenieFrameMainProxy(QWidget *parent)
{
    setWindowOpacity(0.0);
   resize(0,1);
   setWindowTitle(WINDOW_TITLE);
   setWindowFlags(Qt::Window| Qt::MSWindowsFixedSizeDialogHint);
}

void	QGenieFrameMainProxy::closeEvent ( QCloseEvent * event )
{
    showMinimized();
    hide();
}
#endif






