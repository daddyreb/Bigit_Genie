#ifndef IVIEWLAYOUT_H
#define IVIEWLAYOUT_H

/**
 * Represents the layout info for a view or placeholder in an {@link IPageLayout}.
 * <p>
 * This interface is not intended to be implemented by clients.
 * </p>
 * 
 * @since 3.0
 * @noimplement This interface is not intended to be implemented by clients.
 */
class IViewLayout {

    /**
     * Returns whether the view is closeable.
     * The default is <code>true</code>.
     * 
     * @return <code>true</code> if the view is closeable, <code>false</code> if not
     */
public:
	virtual bool isCloseable()=0;

    /**
     * Sets whether the view is closeable.
     * 
     * @param closeable <code>true</code> if the view is closeable, <code>false</code> if not
     */
    virtual void setCloseable(bool closeable)=0;

    /**
     * Returns whether the view is moveable.
     * The default is <code>true</code>.
     * 
     * @return <code>true</code> if the view is moveable, <code>false</code> if not
     */
     virtual bool isMoveable()=0;

    /**
     * Sets whether the view is moveable.
     * 
     * @param moveable <code>true</code> if the view is moveable, <code>false</code> if not
     */
     virtual void setMoveable(bool moveable)=0;

    /**
     * Returns whether the view is a standalone view.
     * 
     * @see IPageLayout#addStandaloneView
     */
    virtual  bool isStandalone()=0;

    /**
     * Returns whether the view shows its title.
     * This is only applicable to standalone views.
     * 
     * @see IPageLayout#addStandaloneView
     */
     virtual bool getShowTitle()=0;
};
#endif