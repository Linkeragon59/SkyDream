#include "DiffractionProtoModule.h"

#include "TangiblesManager.h"
#include "ScreenManager.h"
#include "StateManager.h"

#include "Debugger_Module.h"

#include "imgui_helpers.h"

GLFWwindow* DiffractionProtoModule::GetMainWindow() const
{
	return myScreenManager ? myScreenManager->GetMainWindow() : nullptr;
}

GLFWwindow* DiffractionProtoModule::GetSecondaryWindow() const
{
	return myScreenManager ? myScreenManager->GetSecondaryWindow() : nullptr;
}

void DiffractionProtoModule::DrawMainMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(Loc::GetLocString("ToolsMenu")))
		{
			myStateManager->DrawMainMenuItems();
			myScreenManager->DrawMainMenuItems();
			ImGui::MenuItem(Loc::GetLocString("TangiblesInfoWindow"), nullptr, &myMenuTangiblesInfo);
			if (ImGui::BeginMenu(Loc::GetLocString("Language")))
			{
				static int lang = (int)Loc::GetLang();
				if (ImGui::RadioButton(Loc::GetLocString("English"), &lang, 0))
					Loc::SetLang(Loc::Lang::En);
				if (ImGui::RadioButton(Loc::GetLocString("French"), &lang, 1))
					Loc::SetLang(Loc::Lang::Fr);
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(Loc::GetLocString("Quit")))
				Core::Facade::GetInstance()->Quit();
			ImGui::EndMenu();
		}

#if DEBUG_BUILD
		if (ImGui::BeginMenu(Loc::GetLocString("DebugMenu")))
		{
			ImGui::MenuItem(Loc::GetLocString("ShowWindowsInfo"), nullptr, &myDebugMenuWindowsInfo);
			ImGui::MenuItem(Loc::GetLocString("ShowTouchInfo"), nullptr, &myDebugMenuTouchInfo);
			ImGui::MenuItem(Loc::GetLocString("ShowImGuiDemo"), nullptr, &myDebugMenuImGuiDemo);
			ImGui::MenuItem(Loc::GetLocString("ShowImPlotDemo"), nullptr, &myDebugMenuImplotDemo);
			ImGui::EndMenu();
		}
#endif

		ImGui::EndMenuBar();
	}

	myStateManager->DrawSettingsWindows();
	myScreenManager->DrawSettingsWindows();

	if (myMenuTangiblesInfo)
		myTangiblesManager->ShowInfo(&myMenuTangiblesInfo);

#if DEBUG_BUILD
	if (myDebugMenuWindowsInfo)
		Debugger::DebuggerModule::GetInstance()->ShowWindowsInfoWindow(&myDebugMenuWindowsInfo);
	if (myDebugMenuTouchInfo)
		Debugger::DebuggerModule::GetInstance()->ShowTouchInfo(&myDebugMenuTouchInfo);

	if (myDebugMenuImGuiDemo)
		ImGui::ShowDemoWindow(&myDebugMenuImGuiDemo);
	if (myDebugMenuImplotDemo)
		ImPlot::ShowDemoWindow(&myDebugMenuImplotDemo);
#endif
}

void DiffractionProtoModule::OnInitialize()
{
	Loc::LoadLocFile("Executables/DiffractionProto/Localization/", "Diffraction");
#if DEBUG_BUILD
	Loc::SetLang(Loc::Lang::En);
#else
	Loc::SetLang(Loc::Lang::Fr);
#endif

	myStateManager = new StateManager();
	myTangiblesManager = new TangiblesManager();
	myScreenManager = new ScreenManager();
}

void DiffractionProtoModule::OnFinalize()
{
	SafeDelete(myScreenManager);
	SafeDelete(myTangiblesManager);
	SafeDelete(myStateManager);
}

void DiffractionProtoModule::OnUpdate(Core::Module::UpdateType aType)
{
	if (aType == Core::Module::UpdateType::EarlyUpdate)
	{
		myTangiblesManager->Update();
	}
	else if (aType == Core::Module::UpdateType::MainUpdate)
	{
		myScreenManager->Update();
	}
	else if (aType == Core::Module::UpdateType::LateUpdate)
	{
		myStateManager->Update();
	}
}
