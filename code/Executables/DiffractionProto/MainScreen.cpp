#include "MainScreen.h"

#include "DiffractionProtoModule.h"
#include "ScreenManager.h"
#include "StateManager.h"

#include "Diffraction_Module.h"
#include "Render_RenderModule.h"

#include "imgui_helpers.h"
#include <format>

namespace
{
	ImVec4 locRBG2HSV(const ImVec4& aLightRBG)
	{
		ImVec4 K = ImVec4(0.f, -1.f / 3.f, 2.f / 3.f, -1.f);
		ImVec4 p = aLightRBG.y < aLightRBG.z ? ImVec4(aLightRBG.z, aLightRBG.y, K.w, K.z) : ImVec4(aLightRBG.y, aLightRBG.z, K.x, K.y);
		ImVec4 q = aLightRBG.x < p.x ? ImVec4(p.x, p.y, p.w, aLightRBG.x) : ImVec4(aLightRBG.x, p.y, p.z, p.x);

		float d = q.x - std::min(q.w, q.y);
		float e = (float)1.0e-10;
		return ImVec4(std::fabs(q.z + (q.w - q.y) / (6.f * d + e)), d / (q.x + e), q.x, 1.f);
	}

	ImVec4 locHSV2RGB(const ImVec4& aLightHSV)
	{
		ImVec4 K = ImVec4(1.f, 2.f / 3.f, 1.f / 3.f, 3.f);
		float tmp;
		ImVec4 p = ImVec4(std::fabs(std::modf(aLightHSV.x + K.x, &tmp) * 6.f - K.w), std::fabs(std::modf(aLightHSV.x + K.y, &tmp) * 6.f - K.w), std::fabs(std::modf(aLightHSV.x + K.z, &tmp) * 6.f - K.w), 1.f);
		return aLightHSV.z * ((1.f - aLightHSV.y) * ImVec4(K.x, K.x, K.x, 1.f) + aLightHSV.y * ImVec4(std::clamp(p.x - K.x, 0.f, 1.f), std::clamp(p.y - K.x, 0.f, 1.f), std::clamp(p.z - K.x, 0.f, 1.f), 1.f));
	}

	ImVec4 locApplyLuminosityToLightColor(const ImVec4& aLightRBG, float aLuminosity)
	{
		ImVec4 lightHSV = locRBG2HSV(aLightRBG);

		float saturation = 1.f;
		float value = 1.f;
		if (aLuminosity <= 1.f)
			value = aLuminosity;
		else
			saturation = 1.f / std::pow(aLuminosity, 0.1f);

		ImVec4 finalLightRGB = locHSV2RGB(ImVec4(lightHSV.x, saturation, 1.f, 1.f));
		return ImVec4(finalLightRGB.x, finalLightRGB.y, finalLightRGB.z, value * aLightRBG.w);
	}
}

MainScreen::MainScreen(const Core::WindowModule::WindowParams& someParams)
	: Screen(someParams, true)
{
	Render::RenderModule::GetInstance()->LoadUserTexture("Executables/DiffractionProto/Textures/TwinseLogo.png", myLogoTextureID, myLogoTextureWidth, myLogoTextureHeight);
}

MainScreen::~MainScreen()
{
	Render::RenderModule::GetInstance()->UnloadUserTexture(myLogoTextureID);
}

void MainScreen::DrawMainMenuItems()
{
	ImGui::MenuItem(Loc::GetLocString("ShowGrid"), nullptr, &myShowGrid);
}

void MainScreen::OnGuiUpdate()
{
	Screen::OnGuiUpdate();

	DiffractionProtoModule::GetInstance()->DrawMainMenuBar();

	switch (DiffractionProtoModule::GetInstance()->GetStateManager()->GetState())
	{
	case StateManager::State::Idle:
		DrawTwinseLogo();
		break;
	case StateManager::State::PointLaser:
	case StateManager::State::SlitDiffraction:
	case StateManager::State::SlitsInterference:
	case StateManager::State::SquareHoleDiffraction:
	case StateManager::State::RoundHoleDiffraction:
	case StateManager::State::RoundHolesInterference:
		DrawDiffractionFigure();
		DrawGrid();
		DrawOPDWindow();
		break;
	default:
		break;
	}
}

void MainScreen::DrawTwinseLogo()
{
	static float titleSpacing = 20.f;
	const char* title = Loc::GetLocString("MainScreenTitle");

	ImVec2 pos = ImVec2(myContentPos.x, myContentPos.y);
	ImVec2 size = ImVec2(myContentSize.x, myContentSize.y);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(pos, pos + size, 0xFFFFFFFF);

	ImVec2 logoSize = size;
	if (const Core::WindowModule::WindowProperties* props = Core::WindowModule::GetInstance()->GetWindowProperties(myWindow))
	{
		logoSize.x = myLogoTextureWidth * props->myContentScaleX;
		logoSize.y = myLogoTextureHeight * props->myContentScaleY;
	}
	ImVec2 logoPos = pos + (size - logoSize) / 2.f;
	ImGui::SetCursorScreenPos(ImVec2(logoPos.x, logoPos.y));
	ImGui::Image(myLogoTextureID, ImVec2(logoSize.x, logoSize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

	ImGui::PushFont(myGui->GetFont(Render::FontType::Title));
	ImGui::SetCursorScreenPos(ImVec2(pos.x + (size.x - ImGui::CalcTextSize(title).x) / 2.f, logoPos.y + logoSize.y + titleSpacing));
	ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), title);
	ImGui::PopFont();
}

void MainScreen::DrawGrid()
{
	if (!myShowGrid)
		return;

	DrawGrid(1.f, 0.05f, 0x08FFFFFF);
	DrawGrid(0.5f, 0.05f, 0x05FFFFFF, 2);
	DrawGrid(0.1f, 0.025f, 0x03FFFFFF, 5);
}

void MainScreen::DrawGrid(float aRatioToCm, float aLinesWidth, uint aColor, uint aSkipModulo)
{
	glm::vec2 physicalSize;
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertSizeToPhysicalSize(myWindow, myContentSize, physicalSize);
	physicalSize /= aRatioToCm;

	glm::vec2 physicalToContentSize = myContentSize / physicalSize;
	glm::vec2 linesCounts = { 2.f * std::floor(physicalSize.x / 2.f) + 1.f , 2.f * std::floor(physicalSize.y / 2.f) + 1.f };

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (aSkipModulo == 0)
	{
		ImVec2 start = ImVec2(myContentPos.x + myContentSize.x / 2.f, myContentPos.y);
		ImVec2 end = ImVec2(myContentPos.x + myContentSize.x / 2.f, myContentPos.y + myContentSize.y);
		draw_list->AddLine(start, end, aColor, aLinesWidth * physicalToContentSize.x);
	}

	for (uint i = 1, e = static_cast<uint>(linesCounts.x / 2.f); i <= e; ++i)
	{
		if (aSkipModulo != 0 && i % aSkipModulo == 0)
			continue;

		ImVec2 start1 = ImVec2(myContentPos.x + myContentSize.x / 2.f + i * physicalToContentSize.x, myContentPos.y);
		ImVec2 end1 = ImVec2(myContentPos.x + myContentSize.x / 2.f + i * physicalToContentSize.x, myContentPos.y + myContentSize.y);
		draw_list->AddLine(start1, end1, aColor, aLinesWidth * physicalToContentSize.x);

		ImVec2 start2 = ImVec2(myContentPos.x + myContentSize.x / 2.f - i * physicalToContentSize.x, myContentPos.y);
		ImVec2 end2 = ImVec2(myContentPos.x + myContentSize.x / 2.f - i * physicalToContentSize.x, myContentPos.y + myContentSize.y);
		draw_list->AddLine(start2, end2, aColor, aLinesWidth * physicalToContentSize.x);
	}

	if (aSkipModulo == 0)
	{
		ImVec2 start = ImVec2(myContentPos.x, myContentPos.y + myContentSize.y / 2.f);
		ImVec2 end = ImVec2(myContentPos.x + myContentSize.x, myContentPos.y + myContentSize.y / 2.f);
		draw_list->AddLine(start, end, aColor, aLinesWidth * physicalToContentSize.x);
	}

	for (uint i = 1, e = static_cast<uint>(linesCounts.x / 2.f); i <= e; ++i)
	{
		if (aSkipModulo != 0 && i % aSkipModulo == 0)
			continue;

		ImVec2 start1 = ImVec2(myContentPos.x, myContentPos.y + myContentSize.y / 2.f + i * physicalToContentSize.y);
		ImVec2 end1 = ImVec2(myContentPos.x + myContentSize.x, myContentPos.y + myContentSize.y / 2.f + i * physicalToContentSize.y);
		draw_list->AddLine(start1, end1, aColor, aLinesWidth * physicalToContentSize.x);

		ImVec2 start2 = ImVec2(myContentPos.x, myContentPos.y + myContentSize.y / 2.f - i * physicalToContentSize.y);
		ImVec2 end2 = ImVec2(myContentPos.x + myContentSize.x, myContentPos.y + myContentSize.y / 2.f - i * physicalToContentSize.y);
		draw_list->AddLine(start2, end2, aColor, aLinesWidth * physicalToContentSize.x);
	}
}

void MainScreen::DrawDiffractionFigure()
{
	glm::vec2 physicalSize;
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertSizeToPhysicalSize(myWindow, myContentSize, physicalSize);

	Diffraction::BaseDiffractionData& data = DiffractionProtoModule::GetInstance()->GetStateManager()->GetDiffractionData();
	data.xrange = physicalSize.x;
	data.yrange = physicalSize.y;

	ImTextureID textureId = Render::RenderModule::GetInstance()->GetUserRenderTargetTexture(Diffraction::DiffractionModule::GetInstance()->GetRenderTarget());
	ImGui::Image(textureId, ImVec2(myContentSize.x, myContentSize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void MainScreen::DrawOPDWindow()
{
	StateManager* stateManager = DiffractionProtoModule::GetInstance()->GetStateManager();

	StateManager::State currentState = stateManager->GetState();
#if ALLOW_OPD_IN_DIFFRACTION
	if (currentState != StateManager::State::SlitDiffraction && currentState != StateManager::State::SlitsInterference)
#else
	if (currentState != StateManager::State::SlitsInterference)
#endif
		return;

	const ExperimentSettings& experimentSettings = stateManager->GetExperimentSettings();
	if (!stateManager->ShouldShowOPD())
		return;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	glm::vec2 userPointPos = glm::vec2(experimentSettings.myUserPointPos, 0.f);
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertPhysicalSizeToSize(myWindow, userPointPos, userPointPos);

	ImVec2 userPointFinalPos = ImVec2(myContentPos.x + myContentSize.x / 2.f + userPointPos.x, myContentPos.y + myContentSize.y / 2.f + userPointPos.y);
	draw_list->AddCircleFilled(userPointFinalPos, 3.f, 0xFFFFFFFF);
	draw_list->AddLine(ImVec2(userPointFinalPos.x, myContentPos.y), ImVec2(userPointFinalPos.x, myContentPos.y + myContentSize.y), 0xFFFFFFFF);

	glm::vec2 labelOffset = glm::vec2(0.15f, -0.35f);
	DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertPhysicalSizeToSize(myWindow, labelOffset, labelOffset);

	ImGui::SetCursorScreenPos(userPointFinalPos + ImVec2(labelOffset.x, labelOffset.y));
	ImGui::Text("M");

#if ALLOW_OPD_IN_DIFFRACTION
	if (currentState == StateManager::State::SlitDiffraction)
	{
		ImVec2 graphSize = ImVec2(200.f, 200.f);
		if (const Core::WindowModule::WindowProperties* props = Core::WindowModule::GetInstance()->GetWindowProperties(myWindow))
		{
			graphSize.x *= props->myContentScaleX;
			graphSize.y *= props->myContentScaleY;
		}

		Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
		Diffraction::SlitsInterferenceData& slitDiffractionData = stateManager->GetSlitDiffractionData();

		float deltaPhaseAtM = 2.f * 3.1416f * slitDiffractionData.slitsWidth * experimentSettings.myUserPointPos * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

		float C1 = 3.1416f * slitDiffractionData.slitsWidth * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

		float sqrtDiffractionAtM = std::fabs(experimentSettings.myUserPointPos) <= FLT_EPSILON ? 1.f : sinf(C1 * experimentSettings.myUserPointPos) / (C1 * experimentSettings.myUserPointPos);
		float diffractionAtM = std::powf(sqrtDiffractionAtM, 2.f);

		glm::u8vec4 laserColor;
		Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);

		ImVec2 startPoint = ImVec2(myContentPos.x + graphSize.x, myContentPos.y + graphSize.y / 2.f);
		ImVec2 endPoint = ImVec2(myContentPos.x, myContentPos.y + graphSize.y / 2.f);
		float amplitude = graphSize.y / 2.f;
		float frequency = 1.f / graphSize.x;
		ImU32 secondaryRaysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 0.5f));
		ImU32 raysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f));
		for (int slit = 0; slit < experimentSettings.myDiffractionOPDRaysCount; ++slit)
		{
			float deltaPhaseForSlit = slit * deltaPhaseAtM / (experimentSettings.myDiffractionOPDRaysCount - 1) - deltaPhaseAtM / 2.f;
			ImGui::DrawLightRaySin(startPoint, endPoint, amplitude, frequency, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, 100, secondaryRaysColor);
		}
		ImGui::DrawLightRaySin(startPoint, endPoint, sqrtDiffractionAtM * amplitude, frequency, experimentSettings.myOPDPhaseShift, 100, raysColor, 3.f);

		ImGui::SetCursorScreenPos(ImVec2(myContentPos.x + graphSize.x, myContentPos.y));
		ImVec2 luminosityIconSize = graphSize;
		ImVec4 luminosityIconTint = ImVec4(1.f, 1.f, 1.f, diffractionAtM);
		ImGui::Image(myLuminosityIconTextureID, ImVec2(luminosityIconSize.x, luminosityIconSize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), luminosityIconTint);
	}
	else if (currentState == StateManager::State::SlitsInterference)
#endif
	{
		ImVec2 OPDWindowSize = ImVec2(std::max(myContentSize.x * 0.4f, 500.f), std::max(myContentSize.y * 0.33f, 250.f));
		ImGui::SetNextWindowSize(OPDWindowSize);
		ImGui::SetNextWindowPos(ImVec2(myContentPos.x + myContentSize.x - OPDWindowSize.x - 50.f, myContentPos.y + 50.f));

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration;
		if (ImGui::Begin("##OPD", nullptr, windowFlags))
		{
			ImGui::PushFont(myGui->GetFont(Render::FontType::Large));

			ImVec2 contentPos = ImGui::GetCursorScreenPos();
			ImVec2 contentSize = ImGui::GetContentRegionAvail();

			Diffraction::BaseDiffractionData& diffractionData = stateManager->GetDiffractionData();
			Diffraction::BaseDiffractionData& secondarySourcePointDiffractionData = stateManager->GetSecondarySourcePointDiffractionData();
			Diffraction::BaseDiffractionData& resultPointDiffractionData = stateManager->GetResultPointDiffractionData();
			Diffraction::SlitsInterferenceData& slitsInterferenceData = stateManager->GetSlitsInterferenceData();

			float padding = 20.f;
			ImVec2 paddedContentSize = ImVec2(contentSize.x - 2.f * padding, contentSize.y - 2.f * padding);
			ImVec2 halfContentSize = ImVec2((paddedContentSize.x - padding) / 2.f, paddedContentSize.y);
			float rowHeight = halfContentSize.y / 3.f - padding / 2.f;

			float textZoneWidth = halfContentSize.x * (1.f / 8.f);
			float graphZoneWidth = halfContentSize.x * (4.f / 8.f);
			float iconZoneWidth = halfContentSize.x * (3.f / 8.f);

			float resultOffsetX = padding + halfContentSize.x + padding;
			float resultPosY = padding + paddedContentSize.y / 2.f;

			float deltaPhaseAtM = 2.f * 3.1416f * (slitsInterferenceData.slitsSpacing * (slitsInterferenceData.slitsCount - 1.f)) * experimentSettings.myUserPointPos * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

			float C1 = 3.1416f * slitsInterferenceData.slitsWidth * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);
			float C2 = 3.1416f * slitsInterferenceData.slitsSpacing * 10.f / (diffractionData.waveLength * diffractionData.screenDistance);

			float sqrtDiffractionAtM = std::fabs(experimentSettings.myUserPointPos) <= FLT_EPSILON ? 1.f : sinf(C1 * experimentSettings.myUserPointPos) / (C1 * experimentSettings.myUserPointPos);
			float diffractionAtM = std::powf(sqrtDiffractionAtM, 2.f);
			// For now don't consider diffraction in the visual feedback
			diffractionAtM = 1.f;

			float sin1 = sinf(C2 * experimentSettings.myUserPointPos);
			float sin2 = sinf(slitsInterferenceData.slitsCount * C2 * experimentSettings.myUserPointPos);
			float sqrtInterferenceAtM = std::fabs(experimentSettings.myUserPointPos) <= FLT_EPSILON ? 1.f : sin2 / (slitsInterferenceData.slitsCount * sin1);
			float interferenceAtM = std::powf(sqrtInterferenceAtM, 2.f);

			glm::u8vec4 laserColor;
			Diffraction::DiffractionModule::GetInstance()->GetLightSpectrumColor(diffractionData.waveLength, experimentSettings.myWaveLengthRange.myMin, experimentSettings.myWaveLengthRange.myMax, laserColor);

			float amplitude = rowHeight / 2.f * diffractionAtM;
			float frequency = 1.f / graphZoneWidth;
			ImU32 secondaryRaysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 0.5f));
			ImU32 raysColor = ImGui::ColorConvertFloat4ToU32(ImVec4(laserColor.r / 255.f, laserColor.g / 255.f, laserColor.b / 255.f, laserColor.a / 255.f));

			glm::vec2 iconSizePhysical = glm::vec2(iconZoneWidth, iconZoneWidth);
			DiffractionProtoModule::GetInstance()->GetScreenManager()->ConvertSizeToPhysicalSize(myWindow, iconSizePhysical, iconSizePhysical);
			float luminosityFactor = 2.f;
			secondarySourcePointDiffractionData.luminosity = diffractionAtM * luminosityFactor;
			secondarySourcePointDiffractionData.xrange = iconSizePhysical.x;
			secondarySourcePointDiffractionData.yrange = iconSizePhysical.y;
			secondarySourcePointDiffractionData.waveLength = diffractionData.waveLength;
			resultPointDiffractionData.luminosity = diffractionAtM * interferenceAtM * std::pow(slitsInterferenceData.slitsCount, 2.f) * luminosityFactor;
			resultPointDiffractionData.xrange = iconSizePhysical.x;
			resultPointDiffractionData.yrange = iconSizePhysical.y;
			resultPointDiffractionData.waveLength = diffractionData.waveLength;

			// Draw individual curves
			for (int slit = 0; slit < slitsInterferenceData.slitsCount; ++slit)
			{
				float rowPosY = padding + (slit + 0.5f) * (paddedContentSize.y / slitsInterferenceData.slitsCount);

				std::string name = std::format("S{}", slit + 1);
				ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
				ImGui::SetCursorPos(ImVec2(padding + (textZoneWidth - nameSize.x) / 2.f, rowPosY - nameSize.y / 2.f));
				ImGui::Text(name.c_str());

				ImVec2 startPoint = contentPos + ImVec2(padding + textZoneWidth + graphZoneWidth, rowPosY);
				ImVec2 endPoint = contentPos + ImVec2(padding + textZoneWidth, rowPosY);
				float deltaPhaseForSlit = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slit * deltaPhaseAtM / (slitsInterferenceData.slitsCount - 1.f) - deltaPhaseAtM / 2.f;
				ImGui::DrawLightRaySin(startPoint, endPoint, amplitude, frequency, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, 100, raysColor, 2.f);

				if (stateManager->IsRenderingPointTargets())
				{
					ImGui::SetCursorPos(ImVec2(padding + textZoneWidth + graphZoneWidth, rowPosY - iconZoneWidth / 2.f));
					ImTextureID textureId = Render::RenderModule::GetInstance()->GetUserRenderTargetTexture(Diffraction::DiffractionModule::GetInstance()->GetSecondarySourcePointTarget());
					ImGui::Image(textureId, ImVec2(iconZoneWidth, iconZoneWidth), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
				}
			}

			// Draw the result curve
			{
				ImVec2 nameSize = ImGui::CalcTextSize("M");
				float textPosX = resultOffsetX + (textZoneWidth - nameSize.x) / 2.f;
				ImGui::SetCursorPos(ImVec2(textPosX, resultPosY - nameSize.y / 2.f));
				ImGui::Text("M");

				ImVec2 startPoint = contentPos + ImVec2(resultOffsetX + textZoneWidth + graphZoneWidth, resultPosY);
				ImVec2 endPoint = contentPos + ImVec2(resultOffsetX + textZoneWidth, resultPosY);
				for (int slit = 0; slit < slitsInterferenceData.slitsCount; ++slit)
				{
					float deltaPhaseForSlit = std::fabs(slitsInterferenceData.slitsCount - 1.f) <= FLT_EPSILON ? 0.f : slit * deltaPhaseAtM / (slitsInterferenceData.slitsCount - 1.f) - deltaPhaseAtM / 2.f;
					ImGui::DrawLightRaySin(startPoint, endPoint, amplitude, frequency, deltaPhaseForSlit + experimentSettings.myOPDPhaseShift, 100, secondaryRaysColor);
				}
				ImGui::DrawLightRaySin(startPoint, endPoint, sqrtInterferenceAtM * slitsInterferenceData.slitsCount * amplitude, frequency, experimentSettings.myOPDPhaseShift, 100, raysColor, 3.f);

				if (stateManager->IsRenderingPointTargets())
				{
					ImGui::SetCursorPos(ImVec2(resultOffsetX + textZoneWidth + graphZoneWidth, resultPosY - iconZoneWidth / 2.f));
					ImTextureID textureId = Render::RenderModule::GetInstance()->GetUserRenderTargetTexture(Diffraction::DiffractionModule::GetInstance()->GetResultPointTarget());
					ImGui::Image(textureId, ImVec2(iconZoneWidth, iconZoneWidth), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
				}
			}

			ImGui::PopFont();
		}
		ImGui::End();
	}
}
