#ifndef ISELECTIONLISTENER_H
#define ISELECTIONLISTENER_H
class IWorkbenchPart;
class ISelection;
class ISelectionListener
{
public:
virtual void selectionChanged(IWorkbenchPart* part, ISelection* selection)=0;
};
#endif
