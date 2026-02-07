#pragma once

#ifdef IMGUI_SKYDREAM

#include "imgui_helpers.h"

#ifndef IMGUI_DISABLE

#include "imgui_internal.h"

void ImGui::DrawDottedLine(const ImVec2& aStart, const ImVec2& anEnd, float aRepeatLen, float aDrawRatio, ImU32 aColor, float aThickness)
{
    DrawDottedLineShifted(aStart, anEnd, aRepeatLen, aDrawRatio, 0.f, aColor, aThickness);
}

IMGUI_API void ImGui::DrawDottedLineShifted(const ImVec2& aStart, const ImVec2& anEnd, float aRepeatLen, float aDrawRatio, float aShiftRatio, ImU32 aColor, float aThickness)
{
    ImVec2 u = anEnd - aStart;
    float length = ImSqrt(u.x * u.x + u.y * u.y);

    if (length <= FLT_EPSILON)
        return;

    u /= length;

    ImGuiWindow* window = GetCurrentWindow();

    aDrawRatio = std::clamp(aDrawRatio, 0.f, 1.f);
    aShiftRatio = std::clamp(aShiftRatio, 0.f, 1.f);

    ImVec2 start = aStart;
    float drawnLength = 0.f;
    if (aShiftRatio >= 0.f)
    {
        float drawLen = std::clamp(aRepeatLen * aDrawRatio - aRepeatLen * aShiftRatio, 0.f, length);
        window->DrawList->AddLine(start, start + drawLen * u, aColor, aThickness);
        start = start + aRepeatLen * (1.f - aShiftRatio) * u;
        drawnLength += aRepeatLen * (1.f - aShiftRatio);
    }
    while (drawnLength < length)
    {
        float drawLen = std::min(aRepeatLen * aDrawRatio, length - drawnLength);
        window->DrawList->AddLine(start, start + drawLen * u, aColor, aThickness);
        start = start + aRepeatLen * u;
        drawnLength += aRepeatLen;
    }
}

void ImGui::DrawLightRaySin(const ImVec2& aStart, const ImVec2& anEnd, float anAmplitude, float aFrequency, float aPhaseAtStart, int aSegmentsCount, ImU32 aColor, float aThickness)
{
    if (aSegmentsCount <= 0)
        return;

    ImVec2 u = anEnd - aStart;
    float length = ImSqrt(u.x * u.x + u.y * u.y);

    if (length <= FLT_EPSILON)
        return;

    u /= length;
    ImVec2 v = ImVec2(u.y, -u.x);

    ImGuiWindow* window = GetCurrentWindow();

    float step = length / aSegmentsCount;
    for (int n = 0; n < aSegmentsCount; n++)
    {
        float x = n * step;
        float y = anAmplitude * ImSin(2.f * 3.1416f * aFrequency * x + aPhaseAtStart);
        ImVec2 segStart = aStart + x * u + y * v;

        x += step;
        y = anAmplitude * ImSin(2.f * 3.1416f * aFrequency * x + aPhaseAtStart);
        ImVec2 segEnd = aStart + x * u + y * v;

        window->DrawList->AddLine(segStart, segEnd, aColor, aThickness);
    }
}

void ImGui::DrawDottedLightRaySin(const ImVec2& aStart, const ImVec2& anEnd, float anAmplitude, float aFrequency, float aPhaseAtStart, int aSegmentsCount, float aSkipSegmentsRatio, ImU32 aColor, float aThickness /*= 1.0f*/)
{
    if (aSegmentsCount <= 0)
        return;

    ImVec2 u = anEnd - aStart;
    float length = ImSqrt(u.x * u.x + u.y * u.y);

    if (length <= FLT_EPSILON)
        return;

    u /= length;
    ImVec2 v = ImVec2(u.y, -u.x);

    ImGuiWindow* window = GetCurrentWindow();

    float step = length / aSegmentsCount;
    float skipCounter = 0.f;
    aSkipSegmentsRatio = std::clamp(aSkipSegmentsRatio, 0.f, 1.f);

    for (int n = 0; n < aSegmentsCount; n++)
    {
        skipCounter += aSkipSegmentsRatio;
        if (skipCounter >= 1.f)
            skipCounter -= 1.f;

        if (skipCounter < aSkipSegmentsRatio)
            continue;

        float x = n * step;
        float y = anAmplitude * ImSin(2.f * 3.1416f * aFrequency * x + aPhaseAtStart);
        ImVec2 segStart = aStart + x * u + y * v;

        x += step;
        y = anAmplitude * ImSin(2.f * 3.1416f * aFrequency * x + aPhaseAtStart);
        ImVec2 segEnd = aStart + x * u + y * v;

        window->DrawList->AddLine(segStart, segEnd, aColor, aThickness);
    }
}

#endif

#endif
