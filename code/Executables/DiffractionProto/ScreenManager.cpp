#include "ScreenManager.h"

#include "MainScreen.h"
#include "SecondaryScreen.h"

#include "Core_CommandLine.h"
#include "Core_WindowModule.h"
#include "Core_FileHelpers.h"

#include "imgui_helpers.h"
#include "rapidjson/document.h"

ScreenManager::ScreenManager()
{
#if DEBUG_BUILD
	myFavorFullscreen = Core::Facade::GetCommandLine()->IsSet("favorfullscreen");
#else
	myFavorFullscreen = true;
#endif

	DetectAvailableMonitors();

	myMonitorCallbackId = Core::WindowModule::GetInstance()->AddMonitorsCountCallback([this](uint) {
		DetectAvailableMonitors();
		if (!myMainMonitor || !mySecondaryMonitor || myMainMonitor == mySecondaryMonitor)
		{
			// We have only one monitor, ensure we aren't fullscreen
			myMainScreen->SetFullScreen(false);
			mySecondaryScreen->SetFullScreen(false);
		}
	});

	bool shouldOpenFullscreen = myFavorFullscreen && myMainMonitor && mySecondaryMonitor && myMainMonitor != mySecondaryMonitor;
	OpenMainScreen(shouldOpenFullscreen);
	OpenSecondaryScreen(shouldOpenFullscreen);
}

ScreenManager::~ScreenManager()
{
	SafeDelete(myMainScreen);
	SafeDelete(mySecondaryScreen);

	Core::WindowModule::GetInstance()->RemoveMonitorsCountCallback(myMonitorCallbackId);
}

void ScreenManager::Update()
{
	myMainScreen->Update();
	mySecondaryScreen->Update();
}

void ScreenManager::ConvertSizeToPhysicalSize(GLFWwindow* aWindow, const glm::vec2& aSize, glm::vec2& anOutPhysicalSize) const
{
	anOutPhysicalSize = aSize;
	if (const Core::WindowModule::WindowProperties* props = Core::WindowModule::GetInstance()->GetWindowProperties(aWindow))
	{
		anOutPhysicalSize.x *= props->myWidth == 0.f ? 0.f : props->myPhysicalWidth / props->myWidth;
		anOutPhysicalSize.y *= props->myHeight == 0.f ? 0.f : props->myPhysicalHeight / props->myHeight;
		if (const MonitorInfo* monitorInfo = GetMonitorInfo(props->myMonitor))
		{
			anOutPhysicalSize.x *= monitorInfo->myWidth == 0.f ? 0.f : monitorInfo->myTrueWidth / monitorInfo->myWidth;
			anOutPhysicalSize.y *= monitorInfo->myHeight == 0.f ? 0.f : monitorInfo->myTrueHeight / monitorInfo->myHeight;
		}
	}
}

void ScreenManager::ConvertPhysicalSizeToSize(GLFWwindow* aWindow, const glm::vec2& aPhysicalSize, glm::vec2& anOutSize) const
{
	anOutSize = aPhysicalSize;
	if (const Core::WindowModule::WindowProperties* props = Core::WindowModule::GetInstance()->GetWindowProperties(aWindow))
	{
		anOutSize.x *= props->myPhysicalWidth == 0.f ? 0.f : props->myWidth / props->myPhysicalWidth;
		anOutSize.y *= props->myPhysicalHeight == 0.f ? 0.f : props->myHeight / props->myPhysicalHeight;
		if (const MonitorInfo* monitorInfo = GetMonitorInfo(props->myMonitor))
		{
			anOutSize.x *= monitorInfo->myTrueWidth == 0.f ? 0.f : monitorInfo->myWidth / monitorInfo->myTrueWidth;
			anOutSize.y *= monitorInfo->myTrueHeight == 0.f ? 0.f : monitorInfo->myHeight / monitorInfo->myTrueHeight;
		}
	}
}

void ScreenManager::DrawMainMenuItems()
{
	ImGui::MenuItem(Loc::GetLocString("DisplaysSetupWindow"), nullptr, &myMenuMonitorsInfo);
	myMainScreen->DrawMainMenuItems();
	mySecondaryScreen->DrawMainMenuItems();
}

void ScreenManager::DrawSettingsWindows()
{
	if (myMenuMonitorsInfo)
		DrawMonitorsInfo();
	myMainScreen->DrawSettingsWindows();
	mySecondaryScreen->DrawSettingsWindows();
}

void ScreenManager::DrawMonitorsInfo()
{
	if (ImGui::Begin(Loc::GetLocString("DisplaysSetupWindow"), &myMenuMonitorsInfo, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::TreeNodeEx(Loc::GetLocString("WindowFullscreenSection"), ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto toggleFullscreen = [this](Screen* aScreen) {
				if (const Core::WindowModule::WindowProperties* props = Core::WindowModule::GetInstance()->GetWindowProperties(aScreen->GetWindow()))
				{
					ImGui::PushID(aScreen);
					ImGui::Text(props->myTitle.c_str());
					ImGui::SameLine();
					if (props->myMode == Core::WindowModule::WindowMode::Fullscreen)
					{
						if (ImGui::Button(Loc::GetLocString("ExitFullscreen")))
							aScreen->SetFullScreen(false);
					}
					else
					{
						if (ImGui::Button(Loc::GetLocString("EnterFullscreen")))
							aScreen->SetFullScreen(true);
					}
					ImGui::PopID();
				}
			};
			toggleFullscreen(myMainScreen);
			toggleFullscreen(mySecondaryScreen);
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx(Loc::GetLocString("MonitorsPhysicalSizeSection"), ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto tweakMonitorSize = [this](GLFWmonitor* aMonitor, const char* aName) {
				auto it = myMonitorsInfo.find(aMonitor);
				if (it != myMonitorsInfo.end())
				{
					ImGui::PushID(it->first);
					ImGui::Text(aName);
					ImGui::InputFloat(Loc::GetLocString("MonitorPhysicalWidth"), &it->second.myTrueWidth, 0.f, 0.f, "%.2f cm");
					ImGui::InputFloat(Loc::GetLocString("MonitorPhysicalHeight"), &it->second.myTrueHeight, 0.f, 0.f, "%.2f cm");
					ImGui::PopID();
				}
			};
			tweakMonitorSize(myMainMonitor, Loc::GetLocString("MainMonitor"));
			tweakMonitorSize(mySecondaryMonitor, Loc::GetLocString("SecondaryMonitor"));
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ScreenManager::OpenMainScreen(bool aFullscreen)
{
	Core::WindowModule::WindowParams params{};
	params.myMonitor = myMainMonitor;
	params.myMode = aFullscreen ?
		Core::WindowModule::WindowMode::Fullscreen :
		Core::WindowModule::WindowMode::Windowed;
	params.myTitle = Loc::GetLocString("MainWindowTitle"); // TODO : Not reacting to language change

	myMainScreen = new MainScreen(params);
}

void ScreenManager::OpenSecondaryScreen(bool aFullscreen)
{
	Core::WindowModule::WindowParams params{};
	params.myMonitor = mySecondaryMonitor;
	params.myMode = aFullscreen ?
		Core::WindowModule::WindowMode::Fullscreen :
		Core::WindowModule::WindowMode::Windowed;
	params.myTitle = Loc::GetLocString("SecondaryWindowTitle"); // TODO : Not reacting to language change
	params.mySupportsTouch = true;

	mySecondaryScreen = new SecondaryScreen(params);
}

void ScreenManager::DetectAvailableMonitors()
{
	std::vector<MonitorInfo> monitorsInfo;
	std::string monitorsCfgStr;
	if (FileHelpers::ReadAsString("Executables/DiffractionProto/Configs/Monitors.json", monitorsCfgStr))
	{
		rapidjson::Document monitorsCfgDoc;
		monitorsCfgDoc.Parse(monitorsCfgStr.c_str());

		static const char* WidthKey = "Width";
		static const char* HeightKey = "Height";
		static const char* TrueWidthKey = "TrueWidth";
		static const char* TrueHeightKey = "TrueHeight";

		for (auto iter = monitorsCfgDoc.MemberBegin(); iter != monitorsCfgDoc.MemberEnd(); ++iter)
		{
			if (!iter->value.IsObject())
				continue;

			auto monitorCfg = iter->value.GetObject();
			if (!monitorCfg.HasMember(WidthKey) || !monitorCfg[WidthKey].IsNumber())
				continue;
			if (!monitorCfg.HasMember(HeightKey) || !monitorCfg[HeightKey].IsNumber())
				continue;
			if (!monitorCfg.HasMember(TrueWidthKey) || !monitorCfg[TrueWidthKey].IsNumber())
				continue;
			if (!monitorCfg.HasMember(TrueHeightKey) || !monitorCfg[TrueHeightKey].IsNumber())
				continue;

			MonitorInfo monitorInfo = {};
			monitorInfo.myWidth = monitorCfg[WidthKey].GetFloat();
			monitorInfo.myHeight = monitorCfg[HeightKey].GetFloat();
			monitorInfo.myTrueWidth = monitorCfg[TrueWidthKey].GetFloat();
			monitorInfo.myTrueHeight = monitorCfg[TrueHeightKey].GetFloat();
			monitorsInfo.push_back(monitorInfo);
		}
	}

	Core::WindowModule* windowModule = Core::WindowModule::GetInstance();
	const Core::WindowModule::MonitorsInfo& monitors = windowModule->GetMonitorsProperties();

	// Remove info about monitors that aren't available anymore
	for (auto it = myMonitorsInfo.begin(); it != myMonitorsInfo.end();)
	{
		if (!monitors.contains(it->first))
		{
			if (myMainMonitor == it->first)
				myMainMonitor = nullptr;
			if (mySecondaryMonitor == it->first)
				mySecondaryMonitor = nullptr;

			it = myMonitorsInfo.erase(it);
			continue;
		}
		++it;
	}

	if (!myMainMonitor)
		myMainMonitor = windowModule->GetMainMonitor();

	// Add info about the new monitors
	for (const auto& monitor : monitors)
	{
		if (!myMonitorsInfo.contains(monitor.first))
		{
			MonitorInfo info;
			info.myWidth = monitor.second.myPhysicalWidth;
			info.myHeight = monitor.second.myPhysicalHeight;
			info.myTrueWidth = info.myWidth;
			info.myTrueHeight = info.myHeight;

			for (const MonitorInfo& monitorInfo : monitorsInfo)
			{
				if (std::fabs(info.myWidth - monitorInfo.myWidth) <= FLT_EPSILON && std::fabs(info.myHeight - monitorInfo.myHeight) <= FLT_EPSILON)
				{
					info.myTrueWidth = monitorInfo.myTrueWidth;
					info.myTrueHeight = monitorInfo.myTrueHeight;
				}
			}

			myMonitorsInfo[monitor.first] = info;

			if (!mySecondaryMonitor || (mySecondaryMonitor == myMainMonitor && monitor.first != myMainMonitor))
				mySecondaryMonitor = monitor.first;
		}
	}
}

const ScreenManager::MonitorInfo* ScreenManager::GetMonitorInfo(GLFWmonitor* aMonitor) const
{
	auto it = myMonitorsInfo.find(aMonitor);
	return (it != myMonitorsInfo.end()) ? &it->second : nullptr;
}
