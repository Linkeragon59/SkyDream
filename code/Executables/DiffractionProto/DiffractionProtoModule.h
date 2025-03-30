#pragma once

#include "Core_Module.h"

struct GLFWwindow;
struct ImPlotContext;

class TangiblesManager;
class ScreenManager;
class StateManager;

class DiffractionProtoModule : public Core::Module
{
	DECLARE_CORE_MODULE(DiffractionProtoModule, "DiffractionProto", { "Diffraction" })

public:
	const StateManager* GetStateManager() const { return myStateManager; }
	StateManager* GetStateManager() { return myStateManager; }
	const ScreenManager* GetScreenManager() const { return myScreenManager; }
	const TangiblesManager* GetTangiblesManager() const { return myTangiblesManager; }
	TangiblesManager* GetTangiblesManager() { return myTangiblesManager; }

	GLFWwindow* GetMainWindow() const;
	GLFWwindow* GetSecondaryWindow() const;

	void DrawMainMenuBar();
#if DEBUG_BUILD
	bool IsShowingDebugTouches() const { return myDebugMenuTouchInfo; }
#endif

protected:
	void OnInitialize() override;
	void OnFinalize() override;
	void OnUpdate(Core::Module::UpdateType aType) override;

private:
	bool myMenuTangiblesInfo = false;
#if DEBUG_BUILD
	bool myDebugMenuWindowsInfo = false;
	bool myDebugMenuTouchInfo = false;
	bool myDebugMenuImGuiDemo = false;
	bool myDebugMenuImplotDemo = false;
#endif

	StateManager* myStateManager = nullptr;
	TangiblesManager* myTangiblesManager = nullptr;
	ScreenManager* myScreenManager = nullptr;
};
