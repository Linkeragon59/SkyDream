#pragma once

#include "Screen.h"

struct GLFWmonitor;
struct GLFWwindow;

class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();

	GLFWwindow* GetMainWindow() const { return myMainScreen->GetWindow(); }
	GLFWwindow* GetSecondaryWindow() const { return mySecondaryScreen->GetWindow(); }

	void Update();

	void ConvertSizeToPhysicalSize(GLFWwindow* aWindow, const glm::vec2& aSize, glm::vec2& anOutPhysicalSize) const;
	void ConvertPhysicalSizeToSize(GLFWwindow* aWindow, const glm::vec2& aPhysicalSize, glm::vec2& anOutSize) const;

	void DrawMainMenuItems();
	void DrawSettingsWindows();
	
private:
	void DrawMonitorsInfo();
	bool myMenuMonitorsInfo = false;

	bool myFavorFullscreen = false;

	void OpenMainScreen(bool aFullscreen);
	void OpenSecondaryScreen(bool aFullscreen);

	void DetectAvailableMonitors();
	uint myMonitorCallbackId = UINT_MAX;
	struct MonitorInfo
	{
		float myWidth = 1.f;
		float myHeight = 1.f;
		float myTrueWidth = 1.f;
		float myTrueHeight = 1.f;
	};
	std::map<GLFWmonitor*, MonitorInfo> myMonitorsInfo;
	const MonitorInfo* GetMonitorInfo(GLFWmonitor* aMonitor) const;

	GLFWmonitor* myMainMonitor = nullptr;
	GLFWmonitor* mySecondaryMonitor = nullptr;

	Screen* myMainScreen;
	Screen* mySecondaryScreen;
};
