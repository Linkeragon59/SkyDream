#pragma once

#include "Diffraction_Data.h"

#include "rapidjson/document.h"

#define ALLOW_OPD_IN_DIFFRACTION 0

struct ExperimentSettings
{
	struct ConfigRange
	{
		float myDefault = 0.f;
		float myMin = 0.f;
		float myMax = 1.f;
	};

	ConfigRange myWaveLengthRange;
	ConfigRange myLuminosityRange;
	ConfigRange myScreenDistanceRange;

	ConfigRange mySlitsCountRange;
	ConfigRange mySlitsSizeRange;
	ConfigRange mySlitsSpacingRange;

	ConfigRange myHolesCountRange;
	ConfigRange myHolesSizeRange;
	ConfigRange myHolesSpacingRange;

	ConfigRange myLightAttenuationRange;

	float myLightAttenuation = 1.f;

	int myCurveResolution = 2000;
	bool myShowCurve = true;
	bool myShowRawDiffractionCurve = false;
	bool myShowRawInterferenceCurve = false;

	bool myShowOPD = false;
	bool myAnimateOPD = true;
	bool myShowOPDZoom = true;
	float myOPDPhaseShift = 0.f;
	int myOPDRayResolution = 500;
	int myDiffractionOPDRaysCount = 20;
	float myUserPointPos = 0.f; // cm
};

class StateManager
{
public:
	enum class State
	{
		Idle,
		PointLaser,
		SlitDiffraction,
		SlitsInterference,
		SquareHoleDiffraction,
		RoundHoleDiffraction,
		RoundHolesInterference,
	};

	StateManager();

	void Update();

	void DrawMainMenuItems();
	void DrawSettingsWindows();

	bool StateJustChanged() const { return myStateJustChanged; }
	void RequestState(State aState) { myNextState = aState; }
	State GetState() const { return myState; }

	Diffraction::BaseDiffractionData& GetDiffractionData() { return myDiffractionData; }
	Diffraction::PointLaserData& GetPointLaserData() { return myPointLaserData; }
	Diffraction::SlitsInterferenceData& GetSlitDiffractionData() { return mySlitDiffractionData; }
	Diffraction::SlitsInterferenceData& GetSlitsInterferenceData() { return mySlitsInterferenceData; }
	Diffraction::SquareHoleDiffractionData& GetSquareHoleDiffractionData() { return mySquareHoleDiffractionData; }
	Diffraction::RoundHolesInterferenceData& GetRoundHoleDiffractionData() { return myRoundHoleDiffractionData; }
	Diffraction::RoundHolesInterferenceData& GetRoundHolesInterferenceData() { return myRoundHolesInterferenceData; }

	bool IsRenderingPointTargets() const { return myIsRenderingPointTargets; }
	Diffraction::BaseDiffractionData& GetSecondarySourcePointDiffractionData() { return mySecondarySourcePointDiffractionData; }
	Diffraction::BaseDiffractionData& GetResultPointDiffractionData() { return myResultPointDiffractionData; }

	const ExperimentSettings& GetExperimentSettings() const { return myExperimentSettings; }
	ExperimentSettings& GetExperimentSettings() { return myExperimentSettings; }

	bool ShouldShowRawDiffractionCurve() const;
	bool ShouldShowRawInterferenceCurve() const;
	bool ShouldShowOPD() const;

private:
	bool LoadConfig();
	bool LoadRangeConfig(const rapidjson::Document& aDoc, const char* aKey, ExperimentSettings::ConfigRange& aRange) const;
	void ResetDiffractionDataToDefault();
	
private:
	bool myStateJustChanged = false;
	State myState = State::Idle;
	State myNextState = State::Idle;
	bool myIsRenderingPointTargets = false;

	Diffraction::BaseDiffractionData myDiffractionData = {};
	Diffraction::PointLaserData myPointLaserData = {};
	Diffraction::SlitsInterferenceData mySlitDiffractionData = {};
	Diffraction::SlitsInterferenceData mySlitsInterferenceData = {};
	Diffraction::SquareHoleDiffractionData mySquareHoleDiffractionData = {};
	Diffraction::RoundHolesInterferenceData myRoundHoleDiffractionData = {};
	Diffraction::RoundHolesInterferenceData myRoundHolesInterferenceData = {};

	Diffraction::BaseDiffractionData mySecondarySourcePointDiffractionData = {};
	Diffraction::BaseDiffractionData myResultPointDiffractionData = {};

	ExperimentSettings myExperimentSettings;
	bool myShowExperimentSettingsWindow = false;
};
