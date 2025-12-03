#include "Core_Facade.h"

#include "Core_FileHelpers.h"
#include "Core_ModuleManager.h"
#include "Core_TimeModule.h"
#include "Core_WindowModule.h"
#include "Core_InputModule.h"
#include "Core_EntityModule.h"

#include "GLFW/glfw3.h"

namespace Core
{
	Facade* Facade::ourInstance = nullptr;

	void Facade::Create(const CreateParams& someParams)
	{
		Assert(!ourInstance);
		ourInstance = new Facade;
		ourInstance->myCommandLine.Parse(someParams.myArgc, someParams.myArgv);
		ourInstance->Initialize();
	}

	void Facade::Destroy()
	{
		Assert(ourInstance);
		ourInstance->Finalize();
		SafeDelete(ourInstance);
	}

	void Facade::Run(GLFWwindow* aWindow)
	{
		Assert(aWindow);
		myMainWindow = aWindow;
		while (!glfwWindowShouldClose(myMainWindow) && !myShouldQuit)
		{
			glfwPollEvents();

			if (!Update())
				break;
		}
		myMainWindow = nullptr;
	}

	Facade::Facade()
	{
		myModuleManager = new ModuleManager();
	}

	Facade::~Facade()
	{
		delete myModuleManager;
	}

	void Facade::Initialize()
	{
		TimeModule::Register();
		WindowModule::Register();
		InputModule::Register();
		EntityModule::Register();
		FilesRedirectionModule::Register();
	}

	void Facade::Finalize()
	{
		FilesRedirectionModule::Unregister();
		EntityModule::Unregister();
		InputModule::Unregister();
		WindowModule::Unregister();
		TimeModule::Unregister();
	}

	bool Facade::Update()
	{
		myModuleManager->Update(Module::UpdateType::EarlyUpdate);
		myModuleManager->Update(Module::UpdateType::MainUpdate);
		myModuleManager->Update(Module::UpdateType::LateUpdate);
		return true;
	}
}
