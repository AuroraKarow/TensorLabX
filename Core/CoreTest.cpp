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
    std::cout
        << Constant->Pi << std::endl;
}

int main()
{
    TestMatrixReferenceEqual();
    TestSingleton();
    std::cout << "All tests run completed!";
    return EXIT_SUCCESS;
}