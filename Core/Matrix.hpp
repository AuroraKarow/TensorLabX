#ifndef __Matrix__
#define __Matrix__
#include "CoreTypes.hpp"
namespace Core {
	namespace Matrix {
		template<typename valueType>
		requires IsArthmetic<valueType>
		class Matrix {
		};
	}
}
#endif // __Matrix__
