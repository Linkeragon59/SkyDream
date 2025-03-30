#pragma once
#include "Core_Module.h"
#include "Core_Entity.h"

struct GLFWwindow;

class AsteorhythmModule : public Core::Module
{
	DECLARE_CORE_MODULE(AsteorhythmModule, "RhythmShooter")

public:
	GLFWwindow* GetWindow() const { return myWindow; }

protected:
	void OnInitialize() override;
	void OnFinalize() override;
	void OnUpdate(UpdateType aType) override;

private:
	GLFWwindow* myWindow = nullptr;

	Core::Entity myCamera;
	Core::Entity mySimpleGeometryTest;
	Core::Entity myTestModel;
	Core::Entity myTestAnimatedModel;
};
