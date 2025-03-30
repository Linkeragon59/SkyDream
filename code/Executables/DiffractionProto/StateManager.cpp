#include "StateManager.h"

#include "DiffractionProtoModule.h"
#include "TangiblesManager.h"

#include "Diffraction_Module.h"

#include "Core_FileHelpers.h"
#include "Core_TimeModule.h"

#include "imgui_helpers.h"

StateManager::StateManager()
{
	LoadConfig();
	ResetDiffractionDataToDefault();
}

void StateManager::Update()
{
	Diffraction::DiffractionModule* diffractionModule = Diffraction::DiffractionModule::GetInstance();
	diffractionModule->SetDiffractionData(myDiffractionData);
	switch (myState)
	{
	case State::PointLaser:
		diffractionModule->SetPointLaserData(myPointLaserData);
		break;
	case State::SlitDiffraction:
		diffractionModule->SetSlitsInterferenceData(mySlitDiffractionData);
		break;
	case State::SlitsInterference:
	{
		diffractionModule->SetSlitsInterferenceData(mySlitsInterferenceData);
		if (ShouldShowOPD())
		{
			diffractionModule->SetSecondarySourcePointDiffractionData(mySecondarySourcePointDiffractionData);
			diffractionModule->SetResultPointDiffractionData(myResultPointDiffractionData);

			Diffraction::PointLaserData secondarySourceAndResultPointLaserData;
			secondarySourceAndResultPointLaserData.attenuation = 0.35f;
			diffractionModule->SetSecondarySourceAndResultPointLaserData(secondarySourceAndResultPointLaserData);
		}
		break;
	}
	case State::SquareHoleDiffraction:
		diffractionModule->SetSquareHoleDiffractionData(mySquareHoleDiffractionData);
		break;
	case State::RoundHoleDiffraction:
		diffractionModule->SetRoundHolesInterferenceData(myRoundHoleDiffractionData);
		break;
	case State::RoundHolesInterference:
		diffractionModule->SetRoundHolesInterferenceData(myRoundHolesInterferenceData);
		break;
	default:
		break;
	}

	if (myNextState != myState)
	{
		switch (myNextState)
		{
		case State::Idle:
			diffractionModule->StartExperiment(Diffraction::DiffractionModule::Experiment::None);
			break;
		case State::PointLaser:
			diffractionModule->StartExperiment(Diffraction::DiffractionModule::Experiment::PointLaser);
			break;
		case State::SlitDiffraction:
		case State::SlitsInterference:
			diffractionModule->StartExperiment(Diffraction::DiffractionModule::Experiment::SlitsInterference);
			break;
		case State::SquareHoleDiffraction:
			diffractionModule->StartExperiment(Diffraction::DiffractionModule::Experiment::SquareHoleDiffraction);
			break;
		case State::RoundHoleDiffraction:
		case State::RoundHolesInterference:
			diffractionModule->StartExperiment(Diffraction::DiffractionModule::Experiment::RoundHolesInterference);
			break;
		default:
			break;
		}

		myState = myNextState;
		myStateJustChanged = true;
	}
	else
	{
		myStateJustChanged = false;
	}

	bool shouldRenderPointTargets = ShouldShowOPD();
	if (myIsRenderingPointTargets != shouldRenderPointTargets)
	{
		diffractionModule->EnablePointTargets(shouldRenderPointTargets);
		myIsRenderingPointTargets = shouldRenderPointTargets;
	}
	
	if (myExperimentSettings.myAnimateOPD)
	{
		myExperimentSettings.myOPDPhaseShift += Core::TimeModule::GetInstance()->GetDeltaTimeSec() * 2.f * 3.1416f;
	}
}

void StateManager::DrawMainMenuItems()
{
	ImGui::MenuItem(Loc::GetLocString("ExperimentSettings"), nullptr, &myShowExperimentSettingsWindow);
}

void StateManager::DrawSettingsWindows()
{
	if (!myShowExperimentSettingsWindow)
		return;

	if (ImGui::Begin(Loc::GetLocString("ExperimentSettings"), &myShowExperimentSettingsWindow, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::SliderFloat(
			Loc::GetLocString("LightAttenuation"),
			&myExperimentSettings.myLightAttenuation,
			myExperimentSettings.myLightAttenuationRange.myMin,
			myExperimentSettings.myLightAttenuationRange.myMax,
			"%.2f m"
		);
		myPointLaserData.attenuation = myExperimentSettings.myLightAttenuation;
		mySlitDiffractionData.attenuation = myExperimentSettings.myLightAttenuation;
		mySlitsInterferenceData.attenuation = myExperimentSettings.myLightAttenuation;

		if (myState != StateManager::State::Idle && myState != StateManager::State::PointLaser)
		{
			ImGui::SliderInt(Loc::GetLocString("CurveResolution"), &myExperimentSettings.myCurveResolution, 1000, 10000);
			ImGui::Checkbox(Loc::GetLocString("ShowResultCurve"), &myExperimentSettings.myShowCurve);
			if ((myState == StateManager::State::SlitsInterference && mySlitsInterferenceData.slitsCount > 1.f)
				|| (myState == StateManager::State::RoundHolesInterference && myRoundHolesInterferenceData.holesCount > 1.f))
			{
				ImGui::Checkbox(Loc::GetLocString("ShowRawDiffraction"), &myExperimentSettings.myShowRawDiffractionCurve);
				ImGui::Checkbox(Loc::GetLocString("ShowRawInterferences"), &myExperimentSettings.myShowRawInterferenceCurve);
			}

#if ALLOW_OPD_IN_DIFFRACTION
			if (myState == StateManager::State::SlitDiffraction || myState == StateManager::State::SlitsInterference)
#else
			if (myState == StateManager::State::SlitsInterference && mySlitsInterferenceData.slitsCount > 1.f && mySlitsInterferenceData.slitsCount <= 3.f)
#endif
			{
				ImGui::Checkbox(Loc::GetLocString("ShowOPD"), &myExperimentSettings.myShowOPD);
				ImGui::Checkbox(Loc::GetLocString("AnimateOPD"), &myExperimentSettings.myAnimateOPD);
				ImGui::Checkbox(Loc::GetLocString("ShowOPDZoom"), &myExperimentSettings.myShowOPDZoom);
				ImGui::SliderInt(Loc::GetLocString("OPDRaysResolution"), &myExperimentSettings.myOPDRayResolution, 100, 2000);
			}

#if ALLOW_OPD_IN_DIFFRACTION
			if (myState == StateManager::State::SlitDiffraction)
			{
				ImGui::SliderInt(Loc::GetLocString("DiffractionOPDRaysCount"), &myExperimentSettings.myDiffractionOPDRaysCount, 2, 50);
			}
#endif
		}
	}
	ImGui::End();
}

bool StateManager::ShouldShowRawDiffractionCurve() const
{
	return ((myState == StateManager::State::SlitsInterference && mySlitsInterferenceData.slitsCount > 1.f)
		|| (myState == StateManager::State::RoundHolesInterference && myRoundHolesInterferenceData.holesCount > 1.f))
		&& myExperimentSettings.myShowRawDiffractionCurve;
}

bool StateManager::ShouldShowRawInterferenceCurve() const
{
	return ((myState == StateManager::State::SlitsInterference && mySlitsInterferenceData.slitsCount > 1.f)
		|| (myState == StateManager::State::RoundHolesInterference && myRoundHolesInterferenceData.holesCount > 1.f))
		&& myExperimentSettings.myShowRawInterferenceCurve;
}

bool StateManager::ShouldShowOPD() const
{
	return myState == StateManager::State::SlitsInterference && mySlitsInterferenceData.slitsCount > 1.f && mySlitsInterferenceData.slitsCount <= 3.f
		&& myExperimentSettings.myShowOPD;
}

bool StateManager::LoadConfig()
{
	std::string diffractionCfgStr;
	if (!FileHelpers::ReadAsString("Executables/DiffractionProto/Configs/Diffraction.json", diffractionCfgStr))
		return false;

	rapidjson::Document diffractionCfgDoc;
	diffractionCfgDoc.Parse(diffractionCfgStr.c_str());

	LoadRangeConfig(diffractionCfgDoc, "WaveLength", myExperimentSettings.myWaveLengthRange);
	LoadRangeConfig(diffractionCfgDoc, "Luminosity", myExperimentSettings.myLuminosityRange);
	LoadRangeConfig(diffractionCfgDoc, "ScreenDistance", myExperimentSettings.myScreenDistanceRange);

	LoadRangeConfig(diffractionCfgDoc, "SlitsCount", myExperimentSettings.mySlitsCountRange);
	LoadRangeConfig(diffractionCfgDoc, "SlitsSize", myExperimentSettings.mySlitsSizeRange);
	LoadRangeConfig(diffractionCfgDoc, "SlitsSpacing", myExperimentSettings.mySlitsSpacingRange);

	LoadRangeConfig(diffractionCfgDoc, "HolesCount", myExperimentSettings.myHolesCountRange);
	LoadRangeConfig(diffractionCfgDoc, "HolesSize", myExperimentSettings.myHolesSizeRange);
	LoadRangeConfig(diffractionCfgDoc, "HolesSpacing", myExperimentSettings.myHolesSpacingRange);

	LoadRangeConfig(diffractionCfgDoc, "LightAttenuation", myExperimentSettings.myLightAttenuationRange);

	ResetDiffractionDataToDefault();

	return true;
}

bool StateManager::LoadRangeConfig(const rapidjson::Document& aDoc, const char* aKey, ExperimentSettings::ConfigRange& aRange) const
{
	if (!aDoc.HasMember(aKey) || !aDoc[aKey].IsObject())
		return false;

	static const char* DefaultKey = "Default";
	static const char* MinKey = "Min";
	static const char* MaxKey = "Max";

	auto range = aDoc[aKey].GetObject();

	if (!range.HasMember(DefaultKey) || !range[DefaultKey].IsNumber())
		return false;
	if (!range.HasMember(MinKey) || !range[MinKey].IsNumber())
		return false;
	if (!range.HasMember(MaxKey) || !range[MaxKey].IsNumber())
		return false;

	aRange.myDefault = range[DefaultKey].GetFloat();
	aRange.myMin = range[MinKey].GetFloat();
	aRange.myMax = range[MaxKey].GetFloat();
	return true;
}

void StateManager::ResetDiffractionDataToDefault()
{
	myDiffractionData.xrange = 0.f;
	myDiffractionData.yrange = 0.f;
	myDiffractionData.screenDistance = myExperimentSettings.myScreenDistanceRange.myDefault;

	myDiffractionData.luminosity = myExperimentSettings.myLuminosityRange.myDefault;
	myDiffractionData.waveLength = myExperimentSettings.myWaveLengthRange.myDefault;
	myDiffractionData.minVisibleWaveLength = myExperimentSettings.myWaveLengthRange.myMin;
	myDiffractionData.maxVisibleWaveLength = myExperimentSettings.myWaveLengthRange.myMax;

	myExperimentSettings.myLightAttenuation = myExperimentSettings.myLightAttenuationRange.myDefault;
	myPointLaserData.attenuation = myExperimentSettings.myLightAttenuationRange.myDefault;

	mySlitDiffractionData.slitsCount = 1.f;
	mySlitDiffractionData.slitsWidth = myExperimentSettings.mySlitsSizeRange.myDefault;
	mySlitDiffractionData.slitsSpacing = 1.f;
	mySlitDiffractionData.attenuation = myExperimentSettings.myLightAttenuationRange.myDefault;

	mySlitsInterferenceData.slitsCount = myExperimentSettings.mySlitsCountRange.myDefault;
	mySlitsInterferenceData.slitsWidth = myExperimentSettings.mySlitsSizeRange.myDefault;
	mySlitsInterferenceData.slitsSpacing = myExperimentSettings.mySlitsSpacingRange.myDefault;
	mySlitsInterferenceData.attenuation = myExperimentSettings.myLightAttenuationRange.myDefault;

	mySquareHoleDiffractionData.holeWidth = myExperimentSettings.myHolesSizeRange.myDefault;
	mySquareHoleDiffractionData.holeHeight = myExperimentSettings.myHolesSizeRange.myDefault;

	myRoundHoleDiffractionData.holesCount = 1.f;
	myRoundHoleDiffractionData.holesDiameter = myExperimentSettings.myHolesSizeRange.myDefault;
	myRoundHoleDiffractionData.holesSpacing = 1.f;

	myRoundHolesInterferenceData.holesCount = myExperimentSettings.myHolesCountRange.myDefault;
	myRoundHolesInterferenceData.holesDiameter = myExperimentSettings.myHolesSizeRange.myDefault;
	myRoundHolesInterferenceData.holesSpacing = myExperimentSettings.myHolesSpacingRange.myDefault;

	mySecondarySourcePointDiffractionData = myDiffractionData;
	mySecondarySourcePointDiffractionData.xrange = 1.f;
	mySecondarySourcePointDiffractionData.yrange = 1.f;
	mySecondarySourcePointDiffractionData.screenDistance = 1.f;
	myResultPointDiffractionData = myDiffractionData;
	myResultPointDiffractionData.xrange = 1.f;
	myResultPointDiffractionData.yrange = 1.f;
	myResultPointDiffractionData.screenDistance = 1.f;
}
