#pragma once

#include "Core_Defines.h"

#define SafeDelete(ptr)	{ if (ptr) { delete ptr; ptr = nullptr; } }

inline uint Align(uint aValue, uint aAlignment)
{
	return (aValue + aAlignment - 1) & ~(aAlignment - 1);
}

#ifdef DEBUG_BUILD
// Use this if you want to get more info in case there is a memory leak
#define newDebug new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define newDebug new
#endif
void InitMemoryLeaksDetection();

namespace Maths
{
	inline bool IsClose(float aValue1, float aValue2, float anEpsilon = FLT_EPSILON)
	{
		return std::abs(aValue1 - aValue2) <= anEpsilon;
	}
}
