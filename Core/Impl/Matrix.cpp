#include "../Matrix.hpp"
namespace Core {
	namespace Data {


		bool MatrixHelper::ElementWiseEqual(void* a, ui32 sizeofA, void* b, ui32 sizeofB) {
			if (a == b && sizeofA == sizeofB)
			{
				return true;
			}

			if (sizeofA != sizeofB)
			{
				return false;
			}

			byte* l = (byte*)a;
			byte* r = (byte*)b;

			for (ui32 i = 0; i < sizeofA; i++)
			{
				if (l[i] != r[i])
				{
					return false;
				}
			}
			return true;
		}
	}
}