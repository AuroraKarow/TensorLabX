#include "CoreTypes.hpp"
#include "Matrix.hpp"
#include <iostream>
using namespace std;
using namespace Core::Data;
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
    std::cout << "Hello World!\n"
              << Constant->Pi;
}

int main()
{
    TestMatrixReferenceEqual();
    return EXIT_SUCCESS;
}