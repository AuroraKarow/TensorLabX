#ifndef __DateTime__CXX20__
#define __DateTime__CXX20__
#include "../../Core/CoreTypes.hpp"
#include <string>
namespace Tool
{
    class DateTime {
    public:
        DateTime(long timeStamp);
        static DateTime Now();
        std::string ToString();
    };
} // namespace Tool


#endif //__DateTime__CXX20__