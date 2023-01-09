// Modernization.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include "Core/CoreTypes.hpp"
#include "../Core/Matrix.hpp"
#include "../Core/Constants.hpp"
#include <iostream>
using namespace Core::Data;

void TestMatrixReferenceEqual();

int main()
{
    /*Matrix<int> m1(5, 5);
    
    Matrix<int> mcopy(m1);
    Matrix<int> mValueAssign = m1;
    _ASSERT(m1.Equal(mcopy) == true);*/
    std::cout << Constant->Pi << std::endl;
    std::cout << Constant << std::endl;
    //TestMatrixReferenceEqual();
    return 0;
}

void TestMatrixReferenceEqual() {
    Matrix<int> m1(5, 5);
    Matrix<int> mcopy(m1);
    std::cout << &m1 << std::endl << &mcopy << std::endl;
    _ASSERT(!m1.Equal(mcopy));
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
