#include "Core_Facade.h"
#include "Render_RenderModule.h"
#include "Diffraction_Module.h"
#include "Debugger_Module.h"
#include "DiffractionProtoModule.h"

int main()
{
	InitMemoryLeaksDetection();

	Core::Facade::CreateParams params;
	params.myArgc = __argc;
	params.myArgv = __argv;
	Core::Facade::Create(params);

	Render::RenderModule::Register();
#if DEBUG_BUILD
	Debugger::DebuggerModule::Register();
#endif
	Localization::LocalizationModule::Register();
	Diffraction::DiffractionModule::Register();
	DiffractionProtoModule::Register();

	Core::Facade::GetInstance()->Run(DiffractionProtoModule::GetInstance()->GetMainWindow());

	DiffractionProtoModule::Unregister();
	Diffraction::DiffractionModule::Unregister();
	Localization::LocalizationModule::Unregister();
#if DEBUG_BUILD
	Debugger::DebuggerModule::Unregister();
#endif
	Render::RenderModule::Unregister();

	Core::Facade::Destroy();

	return EXIT_SUCCESS;
}
