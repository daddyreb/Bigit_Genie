#include <Cocoa/Cocoa.h>

#include "QCoaCoaApplication.h"

#include <QShortcut>
#include <QShortcutEvent>

extern void qt_mac_set_dock_menu(QMenu*);

@interface DockIconClickEventHandler : NSObject
{
@public
    QCoaCoaApplication* macApplication;
}
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end

@implementation DockIconClickEventHandler
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
    if (macApplication)
        macApplication->dockIconClickEvent();
}
@end

class QCoaCoaApplication::Private {
public:
    Private();
    ~Private();
//    NSEvent* cocoaEventFilter( NSEvent* incomingEvent );
    void setupCocoaEventHandler() const;

    NSAutoreleasePool* pool;
//    NSEvent* eventMonitor;
    DockIconClickEventHandler* dockIconClickEventHandler;
};

QCoaCoaApplication::Private::Private()
    : pool( 0 ), /*eventMonitor( 0 ), */dockIconClickEventHandler( 0 )
{
    pool = [[NSAutoreleasePool alloc] init];
    dockIconClickEventHandler = [[DockIconClickEventHandler alloc] init];
//    eventMonitor = [NSEvent
//        addLocalMonitorForEventsMatchingMask:NSKeyDownMask
//        handler:^(NSEvent *incomingEvent) {
//            return cocoaEventFilter(incomingEvent);
//    }];
}

QCoaCoaApplication::Private::~Private()
{
//    [NSEvent removeMonitor:eventMonitor];
    [pool drain];
}
//static int sitemp=0;
//NSEvent* QCoaCoaApplication::Private::cocoaEventFilter( NSEvent* incomingEvent )
//{
//    QTime t=QTime::currentTime();
//    NSUInteger modifierFlags = [incomingEvent modifierFlags];

//    int shortcutFlags = [[incomingEvent charactersIgnoringModifiers] UTF8String][0];

//    if (modifierFlags & NSShiftKeyMask)
//        shortcutFlags |= Qt::ShiftModifier;
//    if (modifierFlags & NSControlKeyMask)
//        shortcutFlags |= Qt::MetaModifier;
//    if (modifierFlags & NSCommandKeyMask)
//        shortcutFlags |= Qt::ControlModifier;
//    if (modifierFlags & NSAlternateKeyMask)
//        shortcutFlags |= Qt::AltModifier;

//    const QKeySequence keySequence( shortcutFlags );
//    const bool autoRepeat = [incomingEvent isARepeat];

//    const QList< QShortcut* > active = activeShortcuts( keySequence, autoRepeat );
//    Q_FOREACH( QShortcut* const shortcut, active )
//    {
//        QShortcutEvent event( keySequence, shortcut->id() );
//        QObject* const receiver = shortcut;
//        receiver->event( &event );
//    }
//    qDebug()<<"sitemp "<<(sitemp++)<<t.msecsTo(QTime::currentTime());

//    if (!active.isEmpty())
//        return nil;

//    return incomingEvent;
//}

void QCoaCoaApplication::Private::setupCocoaEventHandler() const
{
    // TODO: This apparently uses a legacy API and we should be using the
    // applicationShouldHandleReopen:hasVisibleWindows: method on
    // NSApplicationDelegate but this isn't possible without nasty runtime
    // reflection hacks until Qt is fixed. If this breaks, shout at them :)
    [[NSAppleEventManager sharedAppleEventManager]
     setEventHandler:dockIconClickEventHandler
     andSelector:@selector(handleDockClickEvent:withReplyEvent:)
     forEventClass:kCoreEventClass
     andEventID:kAEReopenApplication];
}

QCoaCoaApplication::QCoaCoaApplication( int& argc, char* argv[] )
    : QApplication( argc, argv )
    , m_private( new QCoaCoaApplication::Private() )
{
    m_private->dockIconClickEventHandler->macApplication = this;

//    connect(this, SIGNAL(goToState(State)),
//            this, SLOT(handleStateChange(State)));

//    m_dockMenu.addAction( &m_actionStopAllTasks );
//    m_dockMenu.addSeparator();

//    Q_FOREACH( CharmWindow* window, m_windows )
//        m_dockMenu.addAction( window->showHideAction() );

//    m_dockMenu.addSeparator();
//    m_dockMenu.addMenu( m_timeTracker.menu() );
//    qt_mac_set_dock_menu( &m_dockMenu );

    // OSX doesn't use icons in menus
//    setWindowIcon( QIcon() );
//    Q_FOREACH( CharmWindow* window, m_windows )
//        window->setWindowIcon( QIcon() );
//    m_actionQuit.setIcon( QIcon() );
//    QCoreApplication::setAttribute( Qt::AA_DontShowIconsInMenus );
    QTimer::singleShot(500,this,SLOT(slot_InstallHandler()));

}

void QCoaCoaApplication::slot_InstallHandler()
{
    qDebug()<<"setupCocoaEventHandler";
       m_private->setupCocoaEventHandler();
}

QCoaCoaApplication::~QCoaCoaApplication()
{
    delete m_private;
}


void QCoaCoaApplication::dockIconClickEvent()
{
//    openAWindow();
    //ddddd
    qDebug()<<"!!!!!!!!!";
    QWidgetList wlst=topLevelWidgets();
    foreach(QWidget *w,wlst)
    {
        if(w->windowTitle()=="NETGEAR Genie")
        {
            w->showNormal();
            w->activateWindow();
        }
    }
}

QList< QShortcut* > QCoaCoaApplication::shortcuts( QWidget* parent )
{
    QList< QShortcut* > result;
    if( parent == 0 )
    {
        const QWidgetList widgets = QApplication::topLevelWidgets();
        for( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it )
            result += shortcuts( *it );
    }
    else
    {
        const QList< QShortcut* > cuts = parent->findChildren< QShortcut* >();
        for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
            if( (*it)->context() == Qt::ApplicationShortcut )
                result.push_back( *it );

        const QList< QWidget* > children = parent->findChildren< QWidget* >();
        for( QList< QWidget* >::const_iterator it = children.begin(); it != children.end(); ++it )
            result += shortcuts( *it );
    }
    return result;
}

QList< QShortcut* > QCoaCoaApplication::activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent )
{
    const QList< QShortcut* > cuts = shortcuts( parent );
    QList< QShortcut* > result;
    for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
        if( (*it)->context() == Qt::ApplicationShortcut && ((*it)->autoRepeat() == autorep || !autorep ) && (*it)->isEnabled() && (*it)->key().matches( seq ) )
            result.push_back( *it );
    return result;
}

