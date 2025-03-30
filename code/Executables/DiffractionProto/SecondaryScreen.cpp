#include "SecondaryScreen.h"

#include "DiffractionProtoModule.h"
#include "ScreenManager.h"
#include "StateManager.h"
#include "TangiblesManager.h"

#include "Diffraction_Module.h"
#include "Render_RenderModule.h"
#include "Core_InputModule.h"
#include "Core_WindowModule.h"
#include "Core_TimeModule.h"
#include "Debugger_Module.h"
#include "Core_FileHelpers.h"

#include "imgui_helpers.h"

namespace
{
	float locTangibleSizePadding = 50.f;

	bool locFindRayCircleIntersection(const ImVec2& aRayStart, const ImVec2& aRayDirection, const ImVec2& aCircleCenter, float aCircleRadius, ImVec2& anOutIntersection)
	{
		ImVec2 centerToRay = aRayStart - aCircleCenter;
		float b = 2.f * (aRayDirection.x * centerToRay.x + aRayDirection.y * centerToRay.y);
		float c = centerToRay.x * centerToRay.x + centerToRay.y * centerToRay.y - aCircleRadius * aCircleRadius;
		float delta = b * b - 4.f * c;

		if (delta < 0.f)
			return false;

		float sqrtDelta = std::sqrtf(delta);
		float x1 = (- b + sqrtDelta) / 2.f;
		float x2 = (- b - sqrtDelta) / 2.f;
		anOutIntersection = aRayStart + std::max(x1, x2) * aRayDirection;
		return true;
	}
}

SecondaryScreen::SecondaryScreen(const Core::WindowModule::WindowParams& someParams)
	: Screen(someParams, false)
{
	myWindowCloseCallbackId = Core::WindowModule::GetInstance()->AddCloseCallback([]() {
		Core::Facade::GetInstance()->Quit();
	}, myWindow);

	myTouchCallbackId = Core::InputModule::GetInstance()->AddTouchCallback([this](uint64 aFingerId, double aXPos, double aYPos, bool anUp) {
		OnTouch(aFingerId, aXPos, aYPos, anUp);
	}, myWindow);

	Render::RenderModule::GetInstance()->LoadUserTexture("Executables/DiffractionProto/Textures/TangibleBase.png", myTangibleBaseTextureID, myTangibleBaseTextureWidth, myTangibleBaseTextureHeight);
}

SecondaryScreen::~SecondaryScreen()
{
	Render::RenderModule::GetInstance()->UnloadUserTexture(myTangibleBaseTextureID);

	Core::InputModule::GetInstance()->RemoveTouchCallback(myTouchCallbackId);
	Core::WindowModule::GetInstance()->RemoveCloseCallback(myWindowCloseCallbackId);
}

void SecondaryScreen::OnGuiUpdate()
{
	Screen::OnGuiUpdate();

	DrawOpticalAxis();
	DrawTangibles();
	DrawZones();
	DrawCurve();
	DrawOPD();

#if DEBUG_BUILD
	if (DiffractionProtoModule::GetInstance()->IsShowingDebugTouches())
		Debugger::DebuggerModule::GetInstance()->DrawTouches(myWindow);
#endif
}

void SecondaryScreen::OnTouch(uint64 aFingerId, double aXPos, double aYPos, bool anUp)
{
	DiffractionProtoModule::GetInstance()->GetTangiblesManager()->OnTouch(aFingerId, aXPos, aYPos, anUp);
}

bool SecondaryScreen::IsTangibleInZone(const Tangible& aTangible, const TangibleZone& aZone)
{
	ImVec2 zoneTopLeft = ImVec2(aZone.myTopLeft.x * myContentSize.x + myContentPos.x, aZone.myTopLeft.y * myContentSize.y + myContentPos.y);
	ImVec2 zoneBottomRight = ImVec2(aZone.myBottomRight.x * myContentSize.x + myContentPos.x, aZone.myBottomRight.y * myContentSize.y + myContentPos.y);

	return aTangible.myPosition.x >= zoneTopLeft.x
		&& aTangible.myPosition.y >= zoneTopLeft.y
		&& aTangible.myPosition.x <= zoneBottomRight.x
		&& aTangible.myPosition.y <= zoneBottomRight.y;
}

float SecondaryScreen::GetTangiblePosXInZone(const Tangible& aTangible, const TangibleZone& aZone)
{
	ImVec2 zoneXRange = ImVec2(aZone.myTopLeft.x * myContentSize.x + myContentPos.x, aZone.myBottomRight.x * myContentSize.x + myContentPos.x);
	return (aTangible.myPosition.x - zoneXRange.x) / (zoneXRange.y - zoneXRange.x);
}

void SecondaryScreen::DrawZones()
{
	// TODO : Rework this, too much occuring in this function, this is not just about drawing the zones

	bool isLaserActive = false;
	bool isSlitActive = false;
	bool isSlitsActive = false;
	bool isSquareHoleActive = false;
	bool isRoundHoleActive = false;
	bool isRoundHolesActive = false;

	Diffraction::BaseDiffractionData& diffractionData = DiffractionProtoModule::GetInstance()->GetStateManager()->GetDiffractionData();
	const ExperimentSettings& experimentSettings = DiffractionProtoModule::GetInstance()->GetStateManager()->GetExperimentSettings();

	if (const TangibleZone* laserZone = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetZone("Laser"))
	{
		const Tangible* laser = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Laser");
		if (laser && IsTangibleInZone(*laser, *laserZone))
		{
			if (std::abs(laser->myOrientation + 90.f) <= 20.f)
				isLaserActive = true;
			else
				DrawZone(*laserZone, Loc::GetLocString("PleaseOrientateTheLaser"));
		}
		else
		{
			DrawZone(*laserZone, Loc::GetLocString("PleasePutALaser"));
		}
	}

	if (const TangibleZone* objectZone = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetZone("Object"))
	{
		if (const Tangible* slit = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slit"))
		{
			if (IsTangibleInZone(*slit, *objectZone))
			{
				isSlitActive = true;
				diffractionData.screenDistance = experimentSettings.myScreenDistanceRange.myMax - GetTangiblePosXInZone(*slit, *objectZone) * (experimentSettings.myScreenDistanceRange.myMax - experimentSettings.myScreenDistanceRange.myMin);
			}
		}
		else if (const Tangible* slits = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slits"))
		{
			if (IsTangibleInZone(*slits, *objectZone))
			{
				isSlitsActive = true;
				diffractionData.screenDistance = experimentSettings.myScreenDistanceRange.myMax - GetTangiblePosXInZone(*slits, *objectZone) * (experimentSettings.myScreenDistanceRange.myMax - experimentSettings.myScreenDistanceRange.myMin);
			}
		}
		else if (const Tangible* squareHole = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("SquareHole"))
		{
			if (IsTangibleInZone(*squareHole, *objectZone))
			{
				isSquareHoleActive = true;
				diffractionData.screenDistance = experimentSettings.myScreenDistanceRange.myMax - GetTangiblePosXInZone(*squareHole, *objectZone) * (experimentSettings.myScreenDistanceRange.myMax - experimentSettings.myScreenDistanceRange.myMin);
			}
		}
		else if (const Tangible* roundHole = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("RoundHole"))
		{
			if (IsTangibleInZone(*roundHole, *objectZone))
			{
				isRoundHoleActive = true;
				diffractionData.screenDistance = experimentSettings.myScreenDistanceRange.myMax - GetTangiblePosXInZone(*roundHole, *objectZone) * (experimentSettings.myScreenDistanceRange.myMax - experimentSettings.myScreenDistanceRange.myMin);
			}
		}
		else if (const Tangible* roundHoles = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("RoundHoles"))
		{
			if (IsTangibleInZone(*roundHoles, *objectZone))
			{
				isRoundHolesActive = true;
				diffractionData.screenDistance = experimentSettings.myScreenDistanceRange.myMax - GetTangiblePosXInZone(*roundHoles, *objectZone) * (experimentSettings.myScreenDistanceRange.myMax - experimentSettings.myScreenDistanceRange.myMin);
			}
		}

		if (!isSlitActive && !isSlitsActive && !isSquareHoleActive && !isRoundHoleActive && !isRoundHolesActive)
			DrawZone(*objectZone, Loc::GetLocString("PleasePutAnObject"));
	}

	StateManager::State nextState = StateManager::State::Idle;
	if (isLaserActive)
	{
		if (isSlitActive)
			nextState = StateManager::State::SlitDiffraction;
		else if (isSlitsActive)
			nextState = StateManager::State::SlitsInterference;
		else if (isSquareHoleActive)
			nextState = StateManager::State::SquareHoleDiffraction;
		else if (isRoundHoleActive)
			nextState = StateManager::State::RoundHoleDiffraction;
		else if (isRoundHolesActive)
			nextState = StateManager::State::RoundHolesInterference;
		else
			nextState = StateManager::State::PointLaser;
	}
	DiffractionProtoModule::GetInstance()->GetStateManager()->RequestState(nextState);
}

void SecondaryScreen::DrawZone(const TangibleZone& aZone, const char* aMessage)
{
	ImVec2 zoneTopLeft = ImVec2(aZone.myTopLeft.x * myContentSize.x + myContentPos.x, aZone.myTopLeft.y * myContentSize.y + myContentPos.y);
	ImVec2 zoneBottomRight = ImVec2(aZone.myBottomRight.x * myContentSize.x + myContentPos.x, aZone.myBottomRight.y * myContentSize.y + myContentPos.y);
	ImVec2 zoneCenter = (zoneTopLeft + zoneBottomRight) / 2.f;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRect(zoneTopLeft, zoneBottomRight, 0x10FFFFFF, 10.f, ImDrawFlags_None, 5.f);

	if (aMessage)
	{
		ImGui::PushFont(myGui->GetFont(Render::FontType::Large));
		ImVec2 textHalfSize = ImGui::CalcTextSize(aMessage) / 2.f;
		ImVec4 textColor = ImVec4(1.f, 1.f, 1.f, std::pow(std::cos(1.5f * Core::TimeModule::GetInstance()->GetTimeSec()), 2.f));
		draw_list->AddText(ImVec2(zoneCenter.x - textHalfSize.x, zoneCenter.y - textHalfSize.y), ImGui::ColorConvertFloat4ToU32(textColor), aMessage);
		ImGui::PopFont();
	}
}

void SecondaryScreen::DrawOpticalAxis()
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddLine(ImVec2(myContentPos.x, myContentPos.y + myContentSize.y / 2.f), ImVec2(myContentPos.x + myContentSize.x, myContentPos.y + myContentSize.y / 2.f), 0x10FFFFFF, 20.f);
}

void SecondaryScreen::DrawCurve()
{
	StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
	StateManager::State currentState = stateManager->GetState();
	if (currentState == StateManager::State::Idle || currentState == StateManager::State::PointLaser)
		return;

	const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

	bool hasCurveToShow = experimentSettings.myShowCurve;
	if (currentState == StateManager::State::SlitsInterference || currentState == StateManager::State::RoundHolesInterference)
		hasCurveToShow |= stateManager->ShouldShowRawDiffractionCurve() | stateManager->ShouldShowRawInterferenceCurve();
	if (!hasCurveToShow)
		return;

	const TangibleZone* curveZone = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetZone("Curve");
	if (!curveZone)
		return;

	ImVec2 zoneTopLeft = ImVec2(curveZone->myTopLeft.x * myContentSize.x + myContentPos.x, curveZone->myTopLeft.y * myContentSize.y + myContentPos.y);
	ImVec2 zoneBottomRight = ImVec2(curveZone->myBottomRight.x * myContentSize.x + myContentPos.x, curveZone->myBottomRight.y * myContentSize.y + myContentPos.y);

	std::vector<float> curveX;
	std::vector<float> curveY;
	std::vector<float> curveRawDiffractionY;
	std::vector<float> curveRawInterferenceY;

	curveX.resize(experimentSettings.myCurveResolution);
	curveY.resize(experimentSettings.myCurveResolution);
	curveRawDiffractionY.resize(experimentSettings.myCurveResolution);
	curveRawInterferenceY.resize(experimentSettings.myCurveResolution);

	Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();

	glm::vec2 physicalSize;
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertSizeToPhysicalSize(myWindow, myContentSize, physicalSize);
	float curveRange = physicalSize.y;

	glm::u8vec4 laserColor;
	Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);
	ImVec4 plotColor = ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f);

	switch (currentState)
	{
	case StateManager::State::SlitDiffraction:
	case StateManager::State::SlitsInterference:
	{
		Diffraction::SlitsInterferenceData& slitDiffractionData = stateManager->GetSlitDiffractionData();
		Diffraction::SlitsInterferenceData& slitsInterferenceData = stateManager->GetSlitsInterferenceData();

		float slitsCount = currentState == StateManager::State::SlitDiffraction ? slitDiffractionData.slitsCount : slitsInterferenceData.slitsCount;
		float slitsWidth = currentState == StateManager::State::SlitDiffraction ? slitDiffractionData.slitsWidth : slitsInterferenceData.slitsWidth;
		float slitsSpacing = currentState == StateManager::State::SlitDiffraction ? slitDiffractionData.slitsSpacing : slitsInterferenceData.slitsSpacing;

		float C1 = 3.1416f * slitsWidth * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
		float C2 = 3.1416f * slitsSpacing * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
		for (int i = 0; i < experimentSettings.myCurveResolution; ++i)
		{
			float x = (curveRange * i) / experimentSettings.myCurveResolution - curveRange / 2.f;
			curveX[i] = x;
		
			float sinc = abs(x) <= FLT_EPSILON ? 1.f : sinf(C1 * x) / (C1 * x);
			curveRawDiffractionY[i] = 1.f - std::powf(sinc, 2.f);
		
			float sin1 = sinf(C2 * x);
			float sin2 = sinf(slitsCount * C2 * x);
			curveRawInterferenceY[i] = 1.f - (abs(x) <= FLT_EPSILON ? 1.f : std::powf(sin2 / (slitsCount * sin1), 2.f));
		
			curveY[i] = 1.f - (1.f - curveRawDiffractionY[i]) * (1.f - curveRawInterferenceY[i]);
		}
		break;
	}
	case StateManager::State::SquareHoleDiffraction:
	{
		Diffraction::SquareHoleDiffractionData& squareHoleDiffractionData = stateManager->GetSquareHoleDiffractionData();

		float C = 3.1416f * squareHoleDiffractionData.holeWidth * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
		for (int i = 0; i < experimentSettings.myCurveResolution; ++i)
		{
			float x = (curveRange * i) / experimentSettings.myCurveResolution - curveRange / 2.f;
			curveX[i] = x;
		
			float sinc = abs(x) <= FLT_EPSILON ? 1.f : sinf(C * x) / (C * x);
			curveY[i] = 1.f - std::powf(sinc, 2.f);
		}
		break;
	}
	case StateManager::State::RoundHoleDiffraction:
	case StateManager::State::RoundHolesInterference:
	{
		Diffraction::RoundHolesInterferenceData& holeDiffractionData = stateManager->GetRoundHoleDiffractionData();
		Diffraction::RoundHolesInterferenceData& holesInterferenceData = stateManager->GetRoundHolesInterferenceData();

		float holesCount = currentState == StateManager::State::RoundHoleDiffraction ? holeDiffractionData.holesCount : holesInterferenceData.holesCount;
		float holesWidth = currentState == StateManager::State::RoundHoleDiffraction ? holeDiffractionData.holesDiameter : holesInterferenceData.holesDiameter;
		float holesSpacing = currentState == StateManager::State::RoundHoleDiffraction ? holeDiffractionData.holesSpacing : holesInterferenceData.holesSpacing;

		float C1 = 3.1416f * holesWidth * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
		float C2 = 3.1416f * holesSpacing * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
		for (int i = 0; i < experimentSettings.myCurveResolution; ++i)
		{
			float x = (curveRange * i) / experimentSettings.myCurveResolution - curveRange / 2.f;
			curveX[i] = x;

			float sinc = abs(x) <= FLT_EPSILON ? 1.f : sinf(C1 * x) / (C1 * x);
			curveRawDiffractionY[i] = 1.f - std::powf(sinc, 2.f);

			float sin1 = sinf(C2 * x);
			float sin2 = sinf(holesCount * C2 * x);
			curveRawInterferenceY[i] = 1.f - (abs(x) <= FLT_EPSILON ? 1.f : std::powf(sin2 / (holesCount * sin1), 2.f));

			curveY[i] = 1.f - (1.f - curveRawDiffractionY[i]) * (1.f - curveRawInterferenceY[i]);
		}
		break;
	}
	default:
		break;
	}

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(zoneTopLeft, zoneBottomRight, 0xFF000000);

	ImGui::SetCursorScreenPos(zoneTopLeft);
	if (ImPlot::BeginPlot("Curve Plots", zoneBottomRight - zoneTopLeft, ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoChild | ImPlotFlags_NoFrame))
	{
		ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.f, 0.f));
		ImPlot::SetupAxes("y", "x", ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
		ImPlot::SetupAxesLimits(-0.01, 1.01, -curveRange / 2.f, curveRange / 2.f, ImPlotCond_Always);

		if (currentState == StateManager::State::SlitsInterference || currentState == StateManager::State::RoundHolesInterference)
		{
			if (stateManager->ShouldShowRawDiffractionCurve())
			{
				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
				ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.f);
				ImPlot::PlotLine("Raw Diffraction Curve", curveRawDiffractionY.data(), curveX.data(), experimentSettings.myCurveResolution);
				ImPlot::PopStyleColor(1);
				ImPlot::PopStyleVar(1);
			}
			if (stateManager->ShouldShowRawInterferenceCurve())
			{
				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.f, 0.7f, 1.f, 1.f));
				ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 0.5f);
				ImPlot::PlotLine("Raw Interference Curve", curveRawInterferenceY.data(), curveX.data(), experimentSettings.myCurveResolution);
				ImPlot::PopStyleColor(1);
				ImPlot::PopStyleVar(1);
			}
		}

		if (experimentSettings.myShowCurve)
		{
			ImPlot::PushStyleColor(ImPlotCol_Line, plotColor);
			ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.f);
			ImPlot::PlotLine("Result Curve", curveY.data(), curveX.data(), experimentSettings.myCurveResolution);
			ImPlot::PopStyleColor(1);
			ImPlot::PopStyleVar(1);
		}

		ImPlot::PopStyleColor(1);
		ImPlot::PopStyleVar(1);
		ImPlot::EndPlot();
	}
}

void SecondaryScreen::DrawOPD()
{
	StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();

	StateManager::State currentState = stateManager->GetState();
#if ALLOW_OPD_IN_DIFFRACTION
	if (currentState != StateManager::State::SlitDiffraction && currentState != StateManager::State::SlitsInterference)
#else
	if (currentState != StateManager::State::SlitsInterference)
#endif
		return;

	ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();
	if (!stateManager->ShouldShowOPD())
		return;
	
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Draw cursor to change the M point used to visualize OPD

	glm::vec2 physicalSize;
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertSizeToPhysicalSize(myWindow, myContentSize, physicalSize);

	ImGui::SetCursorScreenPos(ImVec2(myContentPos.x, myContentPos.y));
	ImGui::VSliderFloat("##v", ImVec2(30.f, myContentSize.y), &experimentSettings.myUserPointPos, physicalSize.y / 2.f, -physicalSize.y / 2.f, "");

	glm::vec2 userPointPos = glm::vec2(-0.2f, experimentSettings.myUserPointPos);
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertPhysicalSizeToSize(myWindow, userPointPos, userPointPos);

	ImVec2 userPointFinalPos = ImVec2(myContentPos.x + myContentSize.x + userPointPos.x, myContentPos.y + myContentSize.y / 2.f + userPointPos.y);
	draw_list->AddCircleFilled(userPointFinalPos, 3.f, 0xFFFFFFFF);
	
	glm::vec2 labelOffset = glm::vec2(-0.35f, -0.5f);
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertPhysicalSizeToSize(myWindow, labelOffset, labelOffset);

	ImGui::SetCursorScreenPos(userPointFinalPos + ImVec2(labelOffset.x, labelOffset.y));
	ImGui::Text("M");

	// Draw the rays interfering in M

#if ALLOW_OPD_IN_DIFFRACTION
	if (currentState == StateManager::State::SlitDiffraction)
	{
		if (const Tangible* slit = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slit"))
		{
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::SlitsInterferenceData& slitDiffractionData = stateManager->GetSlitDiffractionData();

			glm::u8vec4 laserColor;
			Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);
			ImU32 raysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f));

			float deltaPhaseAtM = 2.f * 3.1416f * slitDiffractionData.slitsWidth * experimentSettings.myUserPointPos * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

			float step = slit->myRadius * 2.f / (experimentSettings.myDiffractionOPDRaysCount - 1);
			for (int slitIdx = 0; slitIdx < experimentSettings.myDiffractionOPDRaysCount; ++slitIdx)
			{
				float deltaPhaseForSlit = slitIdx * deltaPhaseAtM / (experimentSettings.myDiffractionOPDRaysCount - 1) - deltaPhaseAtM / 2.f;
				ImVec2 rayStartPoint = ImVec2(slit->myPosition.x, slit->myPosition.y - slit->myRadius + slitIdx * step);
			
				// TODO : Find a better way to set amplitude and frequency multiplier than just hardcoding...
				ImGui::DrawLightRaySin(userPointFinalPos, rayStartPoint, 3.f, 40.f / diffractionData.waveLength, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, experimentSettings.myOPDRayResolution, raysColor);
			}
		}
	}
	else if (currentState == StateManager::State::SlitsInterference)
#endif
	{
		if (const Tangible* slits = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slits"))
		{
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::SlitsInterferenceData& slitsInterferenceData = stateManager->GetSlitsInterferenceData();

			glm::u8vec4 laserColor;
			Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);
			ImU32 raysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f));

			float deltaPhaseAtM = 2.f * 3.1416f * (slitsInterferenceData.slitsSpacing * (slitsInterferenceData.slitsCount - 1.f)) * experimentSettings.myUserPointPos * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

			ImGui::DrawDottedLine(userPointFinalPos, ImVec2(slits->myPosition.x, slits->myPosition.y), 5.f, 7.f, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
			
			float step = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slits->myRadius * 2.f / (slitsInterferenceData.slitsCount - 1.f);
			float stepZoom = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : 100.f / (slitsInterferenceData.slitsCount - 1.f);

			for (int slitIdx = 0; slitIdx < slitsInterferenceData.slitsCount; ++slitIdx)
			{
				float deltaPhaseForSlit = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slitIdx * deltaPhaseAtM / (slitsInterferenceData.slitsCount - 1.f) - deltaPhaseAtM / 2.f;
				float yOffset = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slits->myRadius - slitIdx * step;
				ImVec2 rayStartPoint = ImVec2(slits->myPosition.x, slits->myPosition.y - yOffset);

				// TODO : Find a better way to set amplitude and frequency multiplier than just hardcoding...
				ImGui::DrawLightRaySin(userPointFinalPos, rayStartPoint, 3.f, 20.f / diffractionData.waveLength, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, experimentSettings.myOPDRayResolution, raysColor);
				draw_list->AddLine(userPointFinalPos, rayStartPoint, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
			}

			if (experimentSettings.myShowOPDZoom)
			{
				// TODO : Size are hardcoded for now

				ImVec2 slitsZoomPos = ImVec2(slits->myPosition.x + 150.f, slits->myPosition.y - slits->myRadius - locTangibleSizePadding - 100.f);
				ImVec2 userPointZoomPos = userPointFinalPos - ImVec2(150.f, 150.f);

				draw_list->AddCircleFilled(slitsZoomPos, 100.f, IM_COL32(0x00, 0x00, 0x00, 0xFF));
				draw_list->AddCircleFilled(userPointZoomPos, 100.f, IM_COL32(0x00, 0x00, 0x00, 0xFF));

				if (std::fabs(userPointFinalPos.y - (myContentPos.y + myContentSize.y / 2.f)) > FLT_EPSILON)
				{
					ImVec2 zoomRay1StartPoint = ImVec2(slitsZoomPos.x - 60.f, slitsZoomPos.y - 50.f);
					ImVec2 zoomRay2StartPoint = ImVec2(slitsZoomPos.x - 60.f, slitsZoomPos.y + 50.f);
					ImVec2 zoomRaysEndPoint = userPointFinalPos - ImVec2(slits->myPosition.x, slits->myPosition.y - slits->myRadius) + zoomRay1StartPoint;

					ImVec2 zoomRay1 = zoomRay1StartPoint - zoomRaysEndPoint;
					ImVec2 zoomRay2 = zoomRay2StartPoint - zoomRaysEndPoint;

					if (userPointFinalPos.y < (myContentPos.y + myContentSize.y / 2.f))
					{
						ImVec2 projection = zoomRaysEndPoint + ImVec2Util::Length(zoomRay1) * ImVec2Util::Normalize(zoomRay2);
						ImGui::DrawDottedLine(zoomRay1StartPoint, projection, 5.f, 7.f, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
					}
					else
					{
						ImVec2 projection = zoomRaysEndPoint + ImVec2Util::Length(zoomRay2) * ImVec2Util::Normalize(zoomRay1);
						ImGui::DrawDottedLine(zoomRay2StartPoint, projection, 5.f, 7.f, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
					}
				}

				for (int slitIdx = 0; slitIdx < slitsInterferenceData.slitsCount; ++slitIdx)
				{
					float deltaPhaseForSlit = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slitIdx * deltaPhaseAtM / (slitsInterferenceData.slitsCount - 1.f) - deltaPhaseAtM / 2.f;
					float yOffset = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slits->myRadius - slitIdx * step;
					ImVec2 rayStartPoint = ImVec2(slits->myPosition.x, slits->myPosition.y - yOffset);
					ImVec2 zoomRayDirection = ImVec2Util::Normalize(userPointFinalPos - rayStartPoint);

					// Zoom at Slit Pos
					{
						float zoomYOffset = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : 50.f - slitIdx * stepZoom;
						ImVec2 zoomRayStartPoint = ImVec2(slitsZoomPos.x - 60.f, slitsZoomPos.y - zoomYOffset);
						ImVec2 zoomRayEndPoint = zoomRayStartPoint;
						if (locFindRayCircleIntersection(zoomRayStartPoint, zoomRayDirection, slitsZoomPos, 100.f, zoomRayEndPoint))
						{
							// TODO : Find a better way to set amplitude and frequency multiplier than just hardcoding...
							ImGui::DrawLightRaySin(zoomRayStartPoint, zoomRayEndPoint, 10.f, 5.f / diffractionData.waveLength, -experimentSettings.myOPDPhaseShift, experimentSettings.myOPDRayResolution / 2, raysColor);
							draw_list->AddLine(zoomRayStartPoint, zoomRayEndPoint, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
							ImGui::SetCursorPos(ImVec2(zoomRayStartPoint.x - 15.f, zoomRayStartPoint.y - 10.f));
							ImGui::Text("S%d", slitIdx + 1);
						}
					}

					// Zoom at M Point
					{
						ImVec2 zoomRayStartPoint = ImVec2(userPointZoomPos.x + 75.f, userPointZoomPos.y);
						ImVec2 zoomRayEndPoint = zoomRayStartPoint;
						if (locFindRayCircleIntersection(zoomRayStartPoint, -1.f * zoomRayDirection, userPointZoomPos, 100.f, zoomRayEndPoint))
						{
							// TODO : Find a better way to set amplitude and frequency multiplier than just hardcoding...
							ImGui::DrawLightRaySin(zoomRayStartPoint, zoomRayEndPoint, 10.f, 5.f / diffractionData.waveLength, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, experimentSettings.myOPDRayResolution / 2, raysColor);
							draw_list->AddLine(zoomRayStartPoint, zoomRayEndPoint, IM_COL32(0xFF, 0xFF, 0xFF, 0x3F));
						}
					}
				}

				ImGui::SetCursorPos(ImVec2(userPointZoomPos.x + 80.f, userPointZoomPos.y - 10.f));
				ImGui::Text("M");

				draw_list->AddCircle(slitsZoomPos, 105.f, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF), 0, 10.f);
				draw_list->AddCircle(userPointZoomPos, 105.f, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF), 0, 10.f);
			}
		}
	}
}

void SecondaryScreen::DrawTangibles()
{
	if (const Tangible* laser = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Laser"))
	{
		StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
		Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
		const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

		glm::u8vec4 laserColor;
		Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);
		ImU32 laserColorU32 = ImGui::ColorConvertFloat4ToU32(ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f));

		float laserLength = glm::length(myContentSize);
		glm::vec2 laserEndPos = laser->myPosition - glm::vec2(std::sinf(glm::radians(laser->myOrientation)) * laserLength, std::cosf(glm::radians(laser->myOrientation)) * laserLength);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(laser->myPosition.x, laser->myPosition.y), ImVec2(laserEndPos.x, laserEndPos.y), laserColorU32, 10.f);

		DrawTangible(*laser, [this, &diffractionData, &experimentSettings]() {
			ImGui::SliderFloat(Loc::GetLocString("WaveLength"), &diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, "%.2f nm");
			ImGui::SliderFloat(Loc::GetLocString("Luminosity"), &diffractionData.luminosity, experimentSettings.myLuminosityRange.myMin, experimentSettings.myLuminosityRange.myMax, "%.2f");
		}, laserColorU32);
	}

	if (const Tangible* slit = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slit"))
	{
		DrawTangible(*slit, [this]() {
			StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::SlitsInterferenceData& slitDiffractionData = stateManager->GetSlitDiffractionData();
			const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

			ImGui::SliderFloat(Loc::GetLocString("SlitWidth"), &slitDiffractionData.slitsWidth, experimentSettings.mySlitsSizeRange.myMin, experimentSettings.mySlitsSizeRange.myMax, "%.2f um");
			ImGui::Text("%s %.2f m", Loc::GetLocString("ScreenDistance"), diffractionData.screenDistance);
		});
	}

	if (const Tangible* slits = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("Slits"))
	{
		DrawTangible(*slits, [this]() {
			StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::SlitsInterferenceData& slitsInterferenceData = stateManager->GetSlitsInterferenceData();
			const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

			int slitCount = static_cast<int>(slitsInterferenceData.slitsCount);
			ImGui::SliderInt(Loc::GetLocString("SlitsCount"), &slitCount, static_cast<int>(experimentSettings.mySlitsCountRange.myMin), static_cast<int>(experimentSettings.mySlitsCountRange.myMax));
			slitsInterferenceData.slitsCount = static_cast<float>(slitCount);
			ImGui::SliderFloat(Loc::GetLocString("SlitsWidth"), &slitsInterferenceData.slitsWidth, experimentSettings.mySlitsSizeRange.myMin, experimentSettings.mySlitsSizeRange.myMax, "%.2f um");
			ImGui::SliderFloat(Loc::GetLocString("SlitsSpacing"), &slitsInterferenceData.slitsSpacing, experimentSettings.mySlitsSpacingRange.myMin, experimentSettings.mySlitsSpacingRange.myMax, "%.2f um");
			ImGui::Text("%s %.2f m", Loc::GetLocString("ScreenDistance"), diffractionData.screenDistance);
		});
	}

	if (const Tangible* squareHole = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("SquareHole"))
	{
		DrawTangible(*squareHole, [this]() {
			StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::SquareHoleDiffractionData& squareHoleDiffractionData = stateManager->GetSquareHoleDiffractionData();
			const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

			ImGui::SliderFloat(Loc::GetLocString("HoleWidth"), &squareHoleDiffractionData.holeWidth, experimentSettings.myHolesSizeRange.myMin, experimentSettings.myHolesSizeRange.myMax, "%.2f um");
			ImGui::SliderFloat(Loc::GetLocString("HoleHeight"), &squareHoleDiffractionData.holeHeight, experimentSettings.myHolesSizeRange.myMin, experimentSettings.myHolesSizeRange.myMax, "%.2f um");
			ImGui::Text("%s %.2f m", Loc::GetLocString("ScreenDistance"), diffractionData.screenDistance);
		});
	}

	if (const Tangible* roundHole = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("RoundHole"))
	{
		DrawTangible(*roundHole, [this]() {
			StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::RoundHolesInterferenceData& roundHoleDiffractionData = stateManager->GetRoundHoleDiffractionData();
			const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

			ImGui::SliderFloat(Loc::GetLocString("HoleDiameter"), &roundHoleDiffractionData.holesDiameter, experimentSettings.myHolesSizeRange.myMin, experimentSettings.myHolesSizeRange.myMax, "%.2f um");
			ImGui::Text("%s %.2f m", Loc::GetLocString("ScreenDistance"), diffractionData.screenDistance);
		});
	}

	if (const Tangible* roundHoles = DiffractionProtoModule::GetInstance()->GetTangiblesManager()->GetTangible("RoundHoles"))
	{
		DrawTangible(*roundHoles, [this]() {
			StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();
			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::RoundHolesInterferenceData& roundHolesInterferenceData = stateManager->GetRoundHolesInterferenceData();
			const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();

			int holesCount = static_cast<int>(roundHolesInterferenceData.holesCount);
			ImGui::SliderInt(Loc::GetLocString("HolesCount"), &holesCount, static_cast<int>(experimentSettings.myHolesCountRange.myMin), static_cast<int>(experimentSettings.myHolesCountRange.myMax));
			roundHolesInterferenceData.holesCount = static_cast<float>(holesCount);
			ImGui::SliderFloat(Loc::GetLocString("HolesDiameter"), &roundHolesInterferenceData.holesDiameter, experimentSettings.myHolesSizeRange.myMin, experimentSettings.myHolesSizeRange.myMax, "%.2f um");
			ImGui::SliderFloat(Loc::GetLocString("HolesSpacing"), &roundHolesInterferenceData.holesSpacing, experimentSettings.myHolesSpacingRange.myMin, experimentSettings.myHolesSpacingRange.myMax, "%.2f um");
			ImGui::Text("%s %.2f m", Loc::GetLocString("ScreenDistance"), diffractionData.screenDistance);
		});
	}
}

void SecondaryScreen::DrawTangible(const Tangible& aTangible, std::function<void()> aWindowCallback, uint aColor)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float tangibleHalfSize = aTangible.myRadius + locTangibleSizePadding;
	draw_list->AddImage(myTangibleBaseTextureID,
		ImVec2(aTangible.myPosition.x - tangibleHalfSize, aTangible.myPosition.y - tangibleHalfSize),
		ImVec2(aTangible.myPosition.x + tangibleHalfSize, aTangible.myPosition.y + tangibleHalfSize),
		ImVec2(0.f, 0.f),
		ImVec2(1.f, 1.f),
		aColor);

	const char* tangibleName = Loc::GetLocString(aTangible.myPreset->myName.c_str());

	ImGui::PushFont(myGui->GetFont(Render::FontType::Large));
	ImVec2 textHalfSize = ImGui::CalcTextSize(tangibleName) / 2.f;
	draw_list->AddText(ImVec2(aTangible.myPosition.x - textHalfSize.x, aTangible.myPosition.y - aTangible.myRadius - 2.f * locTangibleSizePadding - textHalfSize.y), 0xFFFFFFFF, tangibleName);
	ImGui::PopFont();

	// TODO : Should only draw the window if the tangible is the one active in its zone
	if (aWindowCallback)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration;
		if (ImGui::Begin(aTangible.myPreset->myName.c_str(), nullptr, windowFlags))
		{
			ImGui::PushFont(myGui->GetFont(Render::FontType::Large));
			aWindowCallback();
			ImGui::PopFont();

			ImVec2 windowHalfSize = ImGui::GetWindowSize() / 2.f;
			ImGui::SetWindowPos(ImVec2(aTangible.myPosition.x - windowHalfSize.x, aTangible.myPosition.y + aTangible.myRadius + 2.f * locTangibleSizePadding));
		}
		ImGui::End();
	}
}
