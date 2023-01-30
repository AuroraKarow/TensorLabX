#ifndef __Core__Matrix__
#define __Core__Matrix__
#pragma once

#include "CoreTypes.hpp"
#include "Constants.hpp"
#include "Helper.hpp"
#include "MemoryBlock.hpp"
#include "MemoryFactory.hpp"
#include <string>
#include <stdexcept>

// #include "../async"
// #include "../net_decimal"

namespace Core
{
	namespace Data
	{

		enum class MatrixOperation
		{
			Multiply,
			Division,
			Pow,
		};

		enum class MatrixSize
		{
			Reg = 0x0004,
			Byte = 0x0008,
			UnRoll = 0x0004,
			UnRollUnit = 0x0010,
			Block = 0x0010,
		};

		template <typename T>
			requires IsArthmetic<T>
		class Matrix
		{
		protected:
			ui64 row;
			ui64 col;
			ui64 elementCount;
			std::shared_ptr<Memory::MemoryBlock<T>> data;

		public:
			Matrix(ui64 row, ui64 col)
			{
				elementCount = row * col;
				this->row = row;
				this->col = col;
				data = Memory::MemoryFactory<T>::Instance()->GetBlock(elementCount);
			}
			Matrix(const Matrix<T> &matrix)
				: Matrix(matrix.row, matrix.col)
			{
				data->CopyFrom(*matrix.data);
			}
			Matrix(Matrix<T> &&matrix)
			{
				this->move(std::move(matrix));
			}

			~Matrix(){
				Memory::MemoryFactory<T>::Instance()->ReturnBlock(data);
			}

			bool Equal(const Matrix<T> &matrix)
			{
				const T *pthis = data->GetRaw();
				const T *pm = matrix.data->GetRaw();
				return Helper::MatrixHelper::ElementWiseEqual(pthis, elementCount * sizeof(T), pm, matrix.elementCount * sizeof(T));
			}
			bool ReferenceEqual(const Matrix<T> &matrix)
			{
				return this == &matrix;
			}

			std::string ToString()
			{
				_ASSERT(data.Get() != nullptr);
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

			void operator=(const Matrix<T> matrix)
			{
				copy(matrix);
			}

			void Reshape(ui64 newRow, ui64 newCol)
			{
				if (newRow * newCol != elementCount)
				{
					throw std::invalid_argument("new size is not equal to the old size!");
				}
				row = newRow;
				col = newCol;
			}
			T Sum()
			{
				T sum = T();
				// T* p = data->GetRaw();
				//  for (ui64 i = 0; i < elementCount; i++)
				//  {
				//  	sum += p[i]
				//  }

				return sum;
			}

			void Padding(i64 top, i64 right, i64 bottom, i64 left) {}
			void Corpping() {}

		private:
			void reset() noexcept
			{
				this->col = 0;
				this->row = 0;
				this->elementCount = 0;
				// this->data.reset();
			}
			void move(Matrix<T> &&_m)
			{
				col = _m.col;
				row = _m.row;
				elementCount = _m.elementCount;
				// data = std::move(_m.data);
			}
			void copy(const Matrix<T> &_m)
			{
				col = _m.col;
				row = _m.row;

				// if (elementCount >= _m.elementCount)
				// {
				// 	data->CopyFrom(_m);
				// }
				// else
				// {
				// 	data = _m.data->Clone();
				// }
				elementCount = _m.elementCount;
			}
		};

	}
}
#endif // __Core__Matrix__
