#pragma once

#include "Screen.h"

// Screen used vertically to display experiment results

class MainScreen : public Screen
{
public:
	MainScreen(const Core::WindowModule::WindowParams& someParams);
	~MainScreen();

	void DrawMainMenuItems() override;

protected:
	void OnGuiUpdate() override;

private:
	void DrawTwinseLogo();

	void DrawGrid();
	void DrawGrid(float aRatioToCm, float aLinesWidth, uint aColor, uint aSkipModulo = 0);
	void DrawDiffractionFigure();
	void DrawOPDWindow();

	void* myLogoTextureID = nullptr;
	uint myLogoTextureWidth = 0;
	uint myLogoTextureHeight = 0;

	bool myShowGrid = true;
};
