#include "Core_Facade.h"
#include "Render_RenderModule.h"
#include "Editor_EditorModule.h"
#include "AsteorhythmModule.h"

int main()
{
	InitMemoryLeaksDetection();

	Core::Facade::CreateParams params;
	params.myArgc = __argc;
	params.myArgv = __argv;
	Core::Facade::Create(params);

	Render::RenderModule::Register();
	Editor::EditorModule::Register();
	AsteorhythmModule::Register();
	
	Core::Facade::GetInstance()->Run(AsteorhythmModule::GetInstance()->GetWindow());

	AsteorhythmModule::Unregister();
	Editor::EditorModule::Unregister();
	Render::RenderModule::Unregister();

	Core::Facade::Destroy();

	return EXIT_SUCCESS;
}
