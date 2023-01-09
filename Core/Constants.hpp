#ifndef __CORE__CONSTANTS__
#define __CORE__CONSTANTS__
#include "Singleton.hpp"

namespace Core {
    /*Runtime Constants*/
    class RConstants : public Singleton<RConstants>
    {
        MAKE_SINGLETON(RConstants)
       
    public:
        RConstants();
	public:
		const double Pi;
    };
}

#define Constant Core::RConstants::Instance()

#endif // !__CORE__CONSTANTS__
