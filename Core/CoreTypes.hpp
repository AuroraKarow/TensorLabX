#ifndef __CoreTypes__
#define __CoreTypes__

#include <type_traits>
#include <typeinfo>
#include <cstdint>
#include <concepts>

typedef int32_t i32;
typedef int64_t i64;

typedef uint32_t ui32;
typedef uint64_t ui64;

typedef unsigned char ubyte;
typedef char sbyte;

typedef float f32;
typedef double f64;
typedef long double f80; //It's 10 bytes long in cpp.

typedef i64 index;

template <typename T>
concept IsArthmetic = std::is_arithmetic<T>::value;

#endif // !__CoreTypes__
