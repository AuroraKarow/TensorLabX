#ifndef __Core__Matrix__
#define __Core__Matrix__
#include "CoreTypes.hpp"
#include "Constants.hpp"

#include <string>

//#include "../async"
//#include "../net_decimal"

namespace Core {
	namespace Data {
		
		enum class MatrixOperation {
			Multiply,
			Division,
			Pow,
		};

		enum class MatrixSize {
			Reg = 0x0004,
			Byte = 0x0008,
			UnRoll = 0x0004,
			UnRollUnit = 0x0010,
			Block = 0x0010,
		};

		class MatrixHelper;

		template<typename T>
			requires IsArthmetic<T>
		class Matrix {
			//friend MatrixHelper;
		protected:
			ui64 row;
			ui64 col;
			ui64 elementCount;
			std::unique_ptr<T[]> data;
		public:
			Matrix(ui64 row, ui64 col) {
				this->elementCount = row * col;
				this->row = row;
				this->col = col;
				data = std::make_unique<T[]>(elementCount);
			}
			Matrix(const Matrix<T>& matrix)
			: Matrix(matrix.row, matrix.col) 
			{
				_ASSERT(matrix.data != nullptr);
				memcpy_s(data.get(), sizeof(T) * elementCount, matrix.data.get(), sizeof(T) * elementCount);
			}
			Matrix(Matrix<T>&& matrix) {
				this->move(std::move(matrix));
			}

			bool Equal(const Matrix<T>& matrix) {
				return MatrixHelper::ElementWiseEqual(this->data.get(), elementCount * sizeof(T), matrix.data.get(), matrix.elementCount * sizeof(T));
			}

			std::string ToString() {
				_ASSERT(data != nullptr);
				std::string res;
				char buffer[14];
				for (ui32 i = 0; i < elementCount; i++)
				{
					if (i > 0 && i % col != 0)
					{
						res.push_back('\n');
					}
					sprintf_s(buffer, 14, "%e");
					res.append(buffer);
				}
				return res;
			}
		private:
			void reset() noexcept {
				this->col = 0;
				this->row = 0;
				this->elementCount = 0;
				this->data.reset();
			}
			void move(Matrix<T>&& _m) {
				col = _m.col;
				row = _m.row;
				elementCount = _m.elementCount;
				data = std::move(_m.data);
			}
		};

		class MatrixHelper {
		public:
			static bool ElementWiseEqual(void* a, ui32 sizeofA, void* b, ui32 sizeofB);
		};
	}
}
#endif // __Core__Matrix__
