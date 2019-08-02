#pragma once

#ifdef _MSC_VER
#	define ISINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#	define ISINLINE __attribute__((always_inline))
#else
#	define inline
#endif