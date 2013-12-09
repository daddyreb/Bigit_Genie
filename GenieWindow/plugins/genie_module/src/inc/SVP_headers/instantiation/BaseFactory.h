#ifndef BASE_FACTORY_H
#define BASE_FACTORY_H

namespace svp { namespace instantiation {

/**
 * This template based class provides an interface
 * for creating instances of type <code>BaseT</code>.
 *
 * @author magr74
 */
template<typename BaseT>
class BaseFactory
{
	public:
		
		/**
		 * Creates instances of type <code>BaseT</code>.
		 *
		 * @return
		 *		An instance of type <code>BaseT</code>.
		 */
		virtual BaseT* newInstance() = 0;
};

}}
#endif
