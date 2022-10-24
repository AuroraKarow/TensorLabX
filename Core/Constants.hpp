#ifndef __CORE__CONSTANTS__
#define __CORE__CONSTANTS__
#include "Singleton.hpp"

namespace Core {
    /*Runtime Constants*/
    class RConstants : public Singleton<RConstants>
    {
        MAKE_SINGLETON(RConstants)
       
    private:
        RConstants();
	public:
		const double Pi;
    };
}

#endif // !__CORE__CONSTANTS__
