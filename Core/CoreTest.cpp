#include "CoreTypes.hpp"
#include "Matrix.hpp"
#include "MemoryStatistics.hpp"
#include <iostream>
using namespace std;
using namespace Core::Data;

void TestMatrix();
void TestMatrixSum();

void TestMatrixReferenceEqual()
{
    Matrix<int> m1(5, 5);
    Matrix<int> mcopy(m1);
    std::cout << &m1 << std::endl
              << &mcopy << std::endl;
    _ASSERT(m1.Equal(mcopy));
}

void TestSingleton() noexcept
{
    std::cout
        << Constant->Pi << std::endl;
}

void TestMemoryStatistics();

int main()
{
    TestMatrix();
    TestSingleton();
    TestMemoryStatistics();
    std::cout << "All tests run completed!\n";
    return EXIT_SUCCESS;
}

void TestMatrix()
{
    TestMatrixReferenceEqual();
    TestMatrixSum();
}

void TestMatrixSum()
{
    Matrix<f32> m(5, 5);
    _ASSERT(m.Sum() == 0);
}

void TestMemoryStatistics()
{
    std::cout << Core::Memory::MemoryStatistics::Instance()->GetInfo();
}