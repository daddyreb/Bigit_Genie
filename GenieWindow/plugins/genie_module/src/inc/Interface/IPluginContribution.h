#ifndef IPLUGINCONTRIBUTION_H
#define IPLUGINCONTRIBUTION_H
#include <QString>

class IPluginContribution
{
public:

    /**
     * @return the local id of the contribution. Must not be <code>null</code>.
     *         This should correspond to the extension-specific identifier for
     *         a given contribution.
     */
    virtual QString getLocalId()=0;

    /**
     * @return the id of the originating plugin. Can be <code>null</code> if
     *         this contribution did not originate from a plugin.
     */
    virtual QString getPluginId()=0;
};
#endif


