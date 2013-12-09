#ifndef IFOLDERLAYOUT_H
#define IFOLDERLAYOUT_H

/**
 * An <code>IFolderLayout</code> is used to define the initial views within a folder.
 * The folder itself is contained within an <code>IPageLayout</code>.
 * <p>
 * This interface is not intended to be implemented by clients.
 * </p>
 *
 * @see IPageLayout#createFolder
 * @noimplement This interface is not intended to be implemented by clients.
 */
#include <QString>
#include "IPlaceholderFolderLayout.h"

class IFolderLayout :public IPlaceholderFolderLayout {
    /**
     * Adds a view with the given compound id to this folder.
     * See the {@link IPageLayout} type documentation for more details about compound ids.
     * The primary id must name a view contributed to the workbench's view extension point 
     * (named <code>"org.eclipse.ui.views"</code>).
     *
     * @param viewId the view id
     */
public:
	virtual void addView(QString viewId)=0;
};
#endif