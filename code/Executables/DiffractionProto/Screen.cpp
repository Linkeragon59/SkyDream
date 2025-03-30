#include "Screen.h"

#include "Render_RenderModule.h"

#include "imgui_helpers.h"

Screen::Screen(const Core::WindowModule::WindowParams& someParams, bool aMenuBar)
{
	myWindow = Core::WindowModule::GetInstance()->OpenWindow(someParams);
	Render::RenderModule::GetInstance()->RegisterWindow(myWindow, Render::RendererType::GuiOnly);

	myGuiEntity = Core::Entity::Create();
	myGui = myGuiEntity.AddComponent<Render::EntityGuiComponent>(myWindow, aMenuBar);
	myGui->myCallback = [this]() { OnGuiUpdate(); };
}

Screen::~Screen()
{
	myGuiEntity.Destroy();

	Render::RenderModule::GetInstance()->UnregisterWindow(myWindow);
	Core::WindowModule::GetInstance()->CloseWindow(myWindow);
}

void Screen::Update()
{
	myGui->Update();
}

void Screen::SetFullScreen(bool aFullScreen)
{
	Core::WindowModule* windowModule = Core::WindowModule::GetInstance();
	if (const Core::WindowModule::WindowProperties* props = windowModule->GetWindowProperties(myWindow))
	{
		if (aFullScreen != (props->myMode == Core::WindowModule::WindowMode::Fullscreen))
		{
			if (aFullScreen)
				windowModule->MakeWindowFullscreen(myWindow);
			else
				windowModule->MakeWindowWindowed(myWindow);
		}
	}
}

void Screen::OnGuiUpdate()
{
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();
	myContentPos = { pos.x, pos.y };
	myContentSize = { size.x, size.y };
}
