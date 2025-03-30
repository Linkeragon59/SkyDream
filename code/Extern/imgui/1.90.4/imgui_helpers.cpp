#pragma once

#ifdef IMGUI_PANDA

#include "imgui_helpers.h"

#ifndef IMGUI_DISABLE

#include "imgui_internal.h"

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

void ImGui::DrawDottedLine(const ImVec2& aStart, const ImVec2& anEnd, float aDotLen, float aSpaceLen, ImU32 aColor, float aThickness)
{
    ImVec2 u = anEnd - aStart;
    float length = ImSqrt(u.x * u.x + u.y * u.y);

    if (length <= FLT_EPSILON)
        return;

    u /= length;

    ImGuiWindow* window = GetCurrentWindow();

    ImVec2 start = aStart;
    float drawnLength = 0.f;
    while (drawnLength < length)
    {
        window->DrawList->AddLine(start, start + std::min(aDotLen, length - drawnLength) * u, aColor, aThickness);

        start = start + (aDotLen + aSpaceLen) * u;
        drawnLength += aDotLen + aSpaceLen;
    }
}

#endif

#endif
