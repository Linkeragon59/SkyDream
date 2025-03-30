#pragma once

#include "Screen.h"

// Screen used horizontally to put tangibles on

struct Tangible;
struct TangibleZone;

class SecondaryScreen : public Screen
{
public:
	SecondaryScreen(const Core::WindowModule::WindowParams& someParams);
	~SecondaryScreen();

protected:
	void OnGuiUpdate() override;

private:
	void OnTouch(uint64 aFingerId, double aXPos, double aYPos, bool anUp);

	bool IsTangibleInZone(const Tangible& aTangible, const TangibleZone& aZone);
	float GetTangiblePosXInZone(const Tangible& aTangible, const TangibleZone& aZone);

	void DrawZones();
	void DrawZone(const TangibleZone& aZone, const char* aMessage);
	void DrawOpticalAxis();

	void DrawCurve();
	void DrawOPD();

	void DrawTangibles();
	void DrawTangible(const Tangible& aTangible, std::function<void()> aWindowCallback, uint aColor = 0xFFFFFFFF);

	uint myWindowCloseCallbackId = UINT_MAX;
	uint myTouchCallbackId = UINT_MAX;

	void* myTangibleBaseTextureID = nullptr;
	uint myTangibleBaseTextureWidth = 0;
	uint myTangibleBaseTextureHeight = 0;
};
