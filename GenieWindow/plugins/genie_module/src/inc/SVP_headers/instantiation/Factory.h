#ifndef FACTORY_H
#define FACTORY_H

namespace svp { namespace instantiation {

/**
 * The <code>Factory</code> class is a template based class
 * which creates instances of type <code>SubT</code> by 
 * default constructor. The type of the created instance
 * has to be an inherited type of type <code>BaseT</code>.
 *
 * See {@link svp::classloading::BaseFactory}
 *
 * @author magr74
 */
template<typename BaseT, typename SubT=BaseT>
class Factory : public BaseFactory<BaseT>
{	
	public:
		/**
		 * Creates instances of type <code>SubT</code> which
		 * inherits base type <code>BaseT</code>.
		 *
		 * @return
		 *		An instance of type <code>BaseT</code>.
		 */
		BaseT* newInstance() { return new SubT; }

};

}}

#endif