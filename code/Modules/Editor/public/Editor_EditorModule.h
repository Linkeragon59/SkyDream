#pragma once
#include "Core_Module.h"

struct GLFWwindow;

namespace Editor
{
	class GraphEditorCanvas;

	class EditorModule : public Core::Module
	{
		DECLARE_CORE_MODULE(EditorModule, "Editor")

	protected:
		void OnInitialize() override;
		void OnFinalize() override;
		void OnUpdate(UpdateType aType) override;

	private:
		void Open();
		void Close();
		uint myOpenCloseCallbackId = UINT_MAX;

		void CallbackUpdate();

		GLFWwindow* myWindow = nullptr;
		//GameCore::CallbackGui* myGui = nullptr;

		GraphEditorCanvas* myCanvas = nullptr;
	};
}
