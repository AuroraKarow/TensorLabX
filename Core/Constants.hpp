#include "Singleton.hpp"
#include <cmath>
#ifndef __CORE__CONSTANTS__
#define __CORE__CONSTANTS__

namespace Core
{
    /*Runtime Constants*/
    class RConstants : public Singleton<RConstants>
    {
        MAKE_SINGLETON(RConstants)

    public:
        RConstants() : Pi(std::acos(-1)) {}

    public:
        const double Pi;
    };
}

#define Constant Core::RConstants::Instance()

#endif // !__CORE__CONSTANTS__
