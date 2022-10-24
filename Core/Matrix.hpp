#ifndef __Matrix__
#define __Matrix__
#include "CoreTypes.hpp"

//#include "../async"
//#include "../net_decimal"

namespace Core {
	namespace Matrix {
		enum class MatrixOperation {
			Multiply,
			Division,
			Pow,
		};


		template<typename valueType>
		requires IsArthmetic<valueType>
		class Matrix {
		protected:
		public:
		};
	}
}
#endif // __Matrix__
