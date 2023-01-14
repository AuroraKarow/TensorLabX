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
            static bool ElementWiseEqual(void *a, ui32 sizeofA, void *b, ui32 sizeofB)
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

                byte *l = (byte *)a;
                byte *r = (byte *)b;

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