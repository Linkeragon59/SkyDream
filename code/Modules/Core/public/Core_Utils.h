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

	inline bool IsClose(const glm::vec2& aValue1, const glm::vec2& aValue2, float anEpsilon = FLT_EPSILON)
	{
		return IsClose(aValue1.x, aValue2.x, anEpsilon) && IsClose(aValue1.y, aValue2.y, anEpsilon);
	}
}
