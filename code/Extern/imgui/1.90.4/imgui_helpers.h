#pragma once

#ifdef IMGUI_PANDA

#include "imgui.h"
#include "implot.h"
#include <algorithm>
#include <cmath>

static inline bool operator==(const ImVec2& aVec1, const ImVec2& aVec2) { return aVec1.x == aVec2.x && aVec1.y == aVec2.y; }
static inline bool operator!=(const ImVec2& aVec1, const ImVec2& aVec2) { return !(aVec1 == aVec2); }
static inline ImVec2 operator*(const float aScalar, const ImVec2& aVec) { return ImVec2(aVec.x * aScalar, aVec.y * aScalar); }
static inline ImVec2 operator*(const ImVec2& aVec, const float aScalar) { return ImVec2(aVec.x * aScalar, aVec.y * aScalar); }
static inline ImVec2 operator/(const ImVec2& aVec, const float aScalar) { return ImVec2(aVec.x / aScalar, aVec.y / aScalar); }
static inline ImVec2 operator+(const ImVec2& aVec1, const ImVec2& aVec2) { return ImVec2(aVec1.x + aVec2.x, aVec1.y + aVec2.y); }
static inline ImVec2 operator-(const ImVec2& aVec1, const ImVec2& aVec2) { return ImVec2(aVec1.x - aVec2.x, aVec1.y - aVec2.y); }
static inline ImVec2 operator*(const ImVec2& aVec1, const ImVec2& aVec2) { return ImVec2(aVec1.x * aVec2.x, aVec1.y * aVec2.y); }
static inline ImVec2 operator/(const ImVec2& aVec1, const ImVec2& aVec2) { return ImVec2(aVec1.x / aVec2.x, aVec1.y / aVec2.y); }
static inline ImVec2& operator*=(const float aScalar, ImVec2& aVec1) { aVec1.x *= aScalar; aVec1.y *= aScalar; return aVec1; }
static inline ImVec2& operator*=(ImVec2& aVec1, const float aScalar) { aVec1.x *= aScalar; aVec1.y *= aScalar; return aVec1; }
static inline ImVec2& operator/=(ImVec2& aVec1, const float aScalar) { aVec1.x /= aScalar; aVec1.y /= aScalar; return aVec1; }
static inline ImVec2& operator+=(ImVec2& aVec1, const ImVec2& aVec2) { aVec1.x += aVec2.x; aVec1.y += aVec2.y; return aVec1; }
static inline ImVec2& operator-=(ImVec2& aVec1, const ImVec2& aVec2) { aVec1.x -= aVec2.x; aVec1.y -= aVec2.y; return aVec1; }
static inline ImVec2& operator*=(ImVec2& aVec1, const ImVec2& aVec2) { aVec1.x *= aVec2.x; aVec1.y *= aVec2.y; return aVec1; }
static inline ImVec2& operator/=(ImVec2& aVec1, const ImVec2& aVec2) { aVec1.x /= aVec2.x; aVec1.y /= aVec2.y; return aVec1; }

static inline ImVec4 operator*(const float aScalar, const ImVec4& aVec) { return ImVec4(aVec.x * aScalar, aVec.y * aScalar, aVec.z * aScalar, aVec.w * aScalar); }
static inline ImVec4 operator+(const ImVec4& aVec1, const ImVec4& aVec2) { return ImVec4(aVec1.x + aVec2.x, aVec1.y + aVec2.y, aVec1.z + aVec2.z, aVec1.w + aVec2.w); }

namespace ImVec2Util
{
    static inline ImVec2 Clamp(const ImVec2& aVec, const ImVec2& aMinVec, const ImVec2& aMaxVec) { return ImVec2(std::clamp(aVec.x, aMinVec.x, aMaxVec.x), std::clamp(aVec.y, aMinVec.y, aMaxVec.y)); }
    static inline float SquareLength(const ImVec2& aVec) { return aVec.x * aVec.x + aVec.y * aVec.y; }
    static inline float Length(const ImVec2& aVec) { return std::sqrtf(SquareLength(aVec)); }
    static inline ImVec2 Normalize(const ImVec2& aVec) { return aVec / Length(aVec); }
}

#ifndef IMGUI_DISABLE

namespace ImGui
{
    IMGUI_API void DrawLightRaySin(const ImVec2& aStart, const ImVec2& anEnd, float anAmplitude, float aFrequency, float aPhaseAtStart, int aSegmentsCount, ImU32 aColor, float aThickness = 1.0f);
    IMGUI_API void DrawDottedLine(const ImVec2& aStart, const ImVec2& anEnd, float aDotLen, float aSpaceLen, ImU32 aColor, float aThickness = 1.0f);
}

#endif

#endif
