#ifndef __CORE__HELPER__
#define __CORE__HELPER__
#include "CoreTypes.hpp"
namespace Core
{
    namespace Helper
    {
        class MatrixHelper
        {
        public:
            static bool ElementWiseEqual(const void *a, ui32 sizeofA, const void *b, ui32 sizeofB)
            {
                if (a == b)
                {
                    return true;
                }

                if (sizeofA != sizeofB)
                {
                    return false;
                }

                if (sizeofA != sizeofB)
                {
                    return false;
                }

                ubyte *l = (ubyte *)a;
                ubyte *r = (ubyte *)b;

                for (ui32 i = 0; i < sizeofA; i++)
                {
                    if (l[i] != r[i])
                    {
                        return false;
                    }
                }
                return true;
            }
        };

    }
}

#endif //__CORE__HELPER__