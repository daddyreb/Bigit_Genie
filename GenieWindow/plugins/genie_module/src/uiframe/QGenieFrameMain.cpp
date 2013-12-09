#include "stable.h"

//use this to fix flick bug
#define SPLASHTIME_AFTER_SHOW 3000
#define BUGMEMORY_TIMER (12*60*60*1000)
#ifdef _WIN32
#include <windows.h>
#endif


QGenieFrameMain::QGenieFrameMain(QWidget *parent)
    :mDrag(false)
    ,mSplash(NULL)
    ,QWidget(parent)
{
    qDebug()<<"QGenieFrameMain::QGenieFrameMain()";
    QFrame *testframe=new QFrame(this);

    mUi.setupUi(testframe);

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(testframe);



    setWindowTitle(WINDOW_TITLE);

#ifdef SHOW_FRAMELESS
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
  //  setWindowFlags(Qt::Window|Qt::CustomizeWindowHint);
  //  this->setAttribute(Qt::WA_TranslucentBackground);
    //mUi.frame_logo->hide();
#else
    mUi.frame_logo->hide(); 
#endif

//    #ifndef USE_DEBUG_PAN
//        mUi.frame_debug->hide();
//    #endif
    mSplashTimer =new QTimer(NULL);

    connect(mSplashTimer,SIGNAL(timeout()),this,SLOT(slot_SplashTimeout()));
    connect(&mDog,SIGNAL(dogwakeup()),this,SLOT(slot_ShowNormal()));

        setMaximumSize(SIZE_HINT_W+4,SIZE_HINT_H+4);
        setMinimumSize(SIZE_HINT_W+4,SIZE_HINT_H+4);
#ifdef   MEMORY_BUG_FIXER
    mMemoryBugTimer=new QTimer(NULL);
    connect(mMemoryBugTimer,SIGNAL(timeout()),this,SLOT(slot_MemoryBugTimer()));
    mMemoryBugTimer->start(BUGMEMORY_TIMER);
#endif
    mDog.start();
#ifdef PLATFORM_MACOSX
    if(QApplication::arguments().contains("-mini"))
    {
        hide();
        showMinimized();
    }
#endif

//    mNotAlwaysOntopTimer=new QTimer(NULL);
 //   connect(mNotAlwaysOntopTimer,SIGNAL(timeout()),this,SLOT(slot_setNotAlwaysOntop()));
    //mNotAlwaysOntopTimer->setSingleShot(true);
}

QGenieFrameMain::~QGenieFrameMain()
{
    qDebug()<<"QGenieFrameMain::~QGenieFrameMain()";
    mSplashTimer->stop();

    SAFE_DELETE(mSplashTimer);
    SAFE_DELETE(mSplash);
#ifdef   MEMORY_BUG_FIXER
    mMemoryBugTimer->stop();
    SAFE_DELETE(mMemoryBugTimer);
#endif
//    mDog.stopit();
    //mNotAlwaysOntopTimer->stop();
  //  SAFE_DELETE(mNotAlwaysOntopTimer);
}

void QGenieFrameMain::startSplash()
{
    if(!mSplash)
    {
      //  QPixmap pixmap(":/splash.png");
#ifdef SELF_SPLASH
        mSplash=new QGenieSplash(NULL);
#else
        QPixmap pixmap(":/splash.png");
        mSplash=new QSplashScreen(pixmap);




#endif

    }

    if(mSplash->pixmap().isNull())
    {
        GENIE_LOG("null splash!!!");
        ASSERT(0);
    }
    mSplash->show();
    this->setWindowOpacity(0.0);
}

void QGenieFrameMain::endSplash()
{
    if(SPLASHTIME_AFTER_SHOW == 0)
    {
        slot_SplashTimeout();
    }
    else
    {
        ASSERT(mSplashTimer);
        mSplashTimer->setSingleShot(true);
        mSplashTimer->start(SPLASHTIME_AFTER_SHOW);
    }
}

void QGenieFrameMain::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && mDrag && !isFullScreen()) {
        move(event->globalPos() - mDragPosition);
#ifdef USE_MAIN_PROXY
        QGenieFrameMainProxy *mainproxy=QGenieFrameMainProxy::GetSingletonPtr();
        ASSERT(mainproxy);
        mainproxy->setGeometry(this->geometry());
#endif

        event->accept();
    }
//    qDebug("mouse move");
}

void QGenieFrameMain::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug("mouse press");
        mDrag=true;
        mDragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void	QGenieFrameMain::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
        qDebug("mouse release");
        mDrag=false;
        event->accept();
    }

}

#ifdef CAN_MAXIMIZE_MAIN
void QGenieFrameMain::mouseDoubleClickEvent ( QMouseEvent * event ) 
{
	if (isFullScreen())
	{
		mUi.frame_logo->show();
	//	setAutoFillBackground(false);
		
		this->setAttribute(Qt::WA_TranslucentBackground,true);
		showNormal();
		qDebug("show normal");
	}
	else
	{
		mUi.frame_logo->hide();
		
	//	this->setAttribute(Qt::WA_TranslucentBackground,false);
		showFullScreen();
		setAutoFillBackground(true);
		
		qDebug("show fullscreen");
	}

}
#endif

void QGenieFrameMain::changeEvent(QEvent* event)
{
    if (event->type()==QEvent::LanguageChange)
    {
       // mUi.retranslateUi(this);
        QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
        ASSERT(traymanager);
        traymanager->changeText();
    }
    QWidget::changeEvent(event);
}

void QGenieFrameMain::slot_SplashTimeout()
{
    if(mSplash)
    {
        //mSplash->finish(NULL);
        //SAFE_DELETE(mSplash);
#ifdef SELF_SPLASH
        mSplash->close();

        mSplash->deleteLater();
        mSplash=NULL;
#else
        mSplash->finish(NULL);
        SAFE_DELETE(mSplash);
#endif
    }
    QGenieLaunchInf *launchinf=QGenieLaunchInf::GetSingletonPtr();
    ASSERT(launchinf);
    this->setWindowOpacity(1.0);
    bool mini=QApplication::arguments().contains("-mini");
    if(!mini)
    {
        slot_ShowNormal();
    }


}

#ifdef MEMORY_BUG_FIXER
void QGenieFrameMain::slot_MemoryBugTimer()
{
    qDebug()<<"QGenieFrameMain::slot_MemoryBugTimer()";
    GENIE_LOG("clear memory for trick!!!");
//    QSize sz=size();
//    QPixmap pixmap(sz);
//    this->render(&pixmap);

//    QRect rect=geometry();
//    QSplashScreen splash(pixmap);
//    splash.move(rect.x(),rect.y());

    if(!isMinimized())
    {
        showMinimized();
        showNormal();
        activateWindow();
    }
   // splash.finish(NULL);
   // mMemoryBugTimer->stop();
}
#endif

void QGenieFrameMain::slot_ShowNormal()
{
    qDebug()<<"QGenieFrameMain::slot_ShowNormal()";
    GENIE_LOG("QGenieFrameMain::slot_ShowNormal()");
#ifdef SIMPLE_FRAME
    return;
#endif
  //  this->setVisible(true);

#if 0
    HWND hMainForm=FindWindowA( "QWidget",WINDOW_TITLE);
      if(hMainForm)
      {
              ShowWindow(hMainForm,   SW_SHOWNORMAL);
              SetForegroundWindow(hMainForm);
      }
      else
      {
          qDebug()<<"can not find window to popup!!!1";
      }

#endif
  // this->activateWindow();
    //mNotAlwaysOntopTimer->setSingleShot(true);
   // mNotAlwaysOntopTimer->start(1000);

//    this->showNormal();
//    this->activateWindow();
#ifdef USE_MAIN_PROXY
   QGenieFrameMainProxy * mainframeproxy=QGenieFrameMainProxy::GetSingletonPtr();
   ASSERT(mainframeproxy);
//   this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
   this->setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
   mainframeproxy->showNormal();
   mainframeproxy->activateWindow();
#else
    //this->hide();
    this->showNormal();
    this->activateWindow();

#endif

#ifdef _WIN32
   HWND hwnd=(HWND)this->winId();
   if(hwnd)
   {

      // 
       SetWindowPos(hwnd,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
       SetWindowPos(hwnd,HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		ShowWindow(hwnd,SW_RESTORE);
		ShowWindow(hwnd,SW_SHOWNORMAL);
       
        SetForegroundWindow(hwnd);
   }
   else
   {
       qDebug()<<"error when SetForegroundWindow(hwnd)";
   }
#endif



    QGenieInterfaceTrayManager *interfacetray=QGenieInterfaceTrayManager::GetSingletonPtr();
    ASSERT(interfacetray);
    interfacetray->notifyTrayMessage(QSystemTrayIcon::Trigger,MFR_NORMALIZE);
}

//void QGenieFrameMain::slot_setNotAlwaysOntop()
//{
//    qDebug()<<"QGenieFrameMain::slot_setNotAlwaysOntop()";
// //    this->setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
//     this->setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
//     this->showNormal();
//
//    // this->showNormal();
//}





