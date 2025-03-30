#pragma once

#include "Core_WindowModule.h"

#include "Core_Entity.h"
#include "Render_EntityRenderComponent.h"

struct GLFWmonitor;
struct GLFWwindow;

class Screen
{
public:
	Screen(const Core::WindowModule::WindowParams& someParams, bool aMenuBar);
	virtual ~Screen();

	GLFWwindow* GetWindow() const { return myWindow; }

	void Update();

	void SetFullScreen(bool aFullScreen);

	virtual void DrawMainMenuItems() {}
	virtual void DrawSettingsWindows() {}

protected:
	virtual void OnGuiUpdate();

	GLFWwindow* myWindow = nullptr;
	Core::Entity myGuiEntity;
	Render::EntityGuiComponent* myGui = nullptr;

	glm::vec2 myContentPos = { 0.f, 0.f };
	glm::vec2 myContentSize = { 0.f, 0.f };
};
