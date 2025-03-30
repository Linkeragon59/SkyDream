#include "AsteorhythmModule.h"

#include "Core_EntityCameraComponent.h"
#include "Core_EntityTransformComponent.h"
#include "Render_EntityRenderComponent.h"
#include "Render_RenderModule.h"
#include "Core_EntityModule.h"
#include "Core_InputModule.h"
#include "Core_WindowModule.h"

void AsteorhythmModule::OnInitialize()
{
	Core::WindowModule::WindowParams params;
	params.myTitle = "Asteorhythm";
	myWindow = Core::WindowModule::GetInstance()->OpenWindow(params);
	Render::RenderModule::GetInstance()->RegisterWindow(myWindow, Render::RendererType::Deferred);

	myCamera = Core::Entity::Create();
	Core::EntityCameraComponent* cameraComponent = myCamera.AddComponent<Core::EntityCameraComponent>(myWindow);
	cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	cameraComponent->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));

	{
		mySimpleGeometryTest = Core::Entity::Create();
		mySimpleGeometryTest.AddComponent<Core::Entity3DTransformComponent>(glm::vec3(-1.5f, 0.0f, 0.0f));
		Render::EntitySimpleGeometryModelComponent* modelComponent = mySimpleGeometryTest.AddComponent<Render::EntitySimpleGeometryModelComponent>();
		modelComponent->FillWithPreset(Render::EntitySimpleGeometryModelComponent::Preset::Sphere);
		modelComponent->myTextureFilename = "Executables/Asteorhythm/Textures/Earth.png";
		modelComponent->Load();
	}

	{
		myTestModel = Core::Entity::Create();
		myTestModel.AddComponent<Core::Entity3DTransformComponent>(glm::vec3(0.0f, 2.5f, 0.0f));
		Render::EntityglTFModelComponent* modelComponent = myTestModel.AddComponent<Render::EntityglTFModelComponent>();
		modelComponent->myFilename = "Executables/Asteorhythm/Models/Cube/Cube.gltf";
		modelComponent->Load();
	}

	{
		myTestAnimatedModel = Core::Entity::Create();
		myTestAnimatedModel.AddComponent<Core::Entity3DTransformComponent>(glm::vec3(0.0f, 0.0f, 1.0f));
		Render::EntityglTFModelComponent* modelComponent = myTestAnimatedModel.AddComponent<Render::EntityglTFModelComponent>();
		modelComponent->myFilename = "Frameworks/Models/CesiumMan/CesiumMan.gltf";
		//modelComponent->myFilename = "Executables/Asteorhythm/Asteroid01/Asteroid01.gltf";
		modelComponent->Load();
	}
}

void AsteorhythmModule::OnFinalize()
{
	myCamera.Destroy();

	mySimpleGeometryTest.Destroy();
	myTestModel.Destroy();
	myTestAnimatedModel.Destroy();

	Render::RenderModule::GetInstance()->UnregisterWindow(myWindow);
	Core::WindowModule::GetInstance()->CloseWindow(myWindow);
}

void AsteorhythmModule::OnUpdate(UpdateType aType)
{
	if (aType == Core::Module::UpdateType::MainUpdate)
	{
		if (Core::Entity3DTransformComponent* component = myTestModel.GetComponent<Core::Entity3DTransformComponent>())
		{
			Core::InputModule* inputModule = Core::InputModule::GetInstance();
			if (inputModule->PollKeyInput(Input::KeyR, nullptr) == Input::Status::Pressed)
			{
				component->Rotate(0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (inputModule->PollKeyInput(Input::KeyE, nullptr) == Input::Status::Pressed)
			{
				component->Scale(glm::vec3(1.01f, 1.0f, 0.99f));
			}
		}
	}
}
