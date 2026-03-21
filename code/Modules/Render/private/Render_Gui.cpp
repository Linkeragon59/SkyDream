#include "Render_Gui.h"

#include "Render_ImGuiHelper.h"

#include "Core_InputModule.h"
#include "Core_WindowModule.h"
#include "Core_TimeModule.h"
#include "Core_FileHelpers.h"
#include "Core_GLFW.h"
#include "stb_image.h"

namespace Render
{
	bool locImGuiMappingInitialized = false;
	int locImGuiMouseMapping[Input::MouseCount];
	ImGuiKey locImGuiKeyMapping[Input::KeyCount];

	void locInitImGuiMapping()
	{
		if (locImGuiMappingInitialized)
			return;
		locImGuiMappingInitialized = true;

		std::memset(&locImGuiMouseMapping, ImGuiMouseButton_Left, Input::MouseCount);
		locImGuiMouseMapping[Input::MouseLeft] = ImGuiMouseButton_Left;
		locImGuiMouseMapping[Input::MouseRight] = ImGuiMouseButton_Right;
		locImGuiMouseMapping[Input::MouseMiddle] = ImGuiMouseButton_Middle;

		std::memset(&locImGuiKeyMapping, ImGuiKey_None, Input::KeyCount);
		locImGuiKeyMapping[Input::Key0] = ImGuiKey_0;
		locImGuiKeyMapping[Input::Key1] = ImGuiKey_1;
		locImGuiKeyMapping[Input::Key2] = ImGuiKey_2;
		locImGuiKeyMapping[Input::Key3] = ImGuiKey_3;
		locImGuiKeyMapping[Input::Key4] = ImGuiKey_4;
		locImGuiKeyMapping[Input::Key5] = ImGuiKey_5;
		locImGuiKeyMapping[Input::Key6] = ImGuiKey_6;
		locImGuiKeyMapping[Input::Key7] = ImGuiKey_7;
		locImGuiKeyMapping[Input::Key8] = ImGuiKey_8;
		locImGuiKeyMapping[Input::Key9] = ImGuiKey_9;

		locImGuiKeyMapping[Input::KeyA] = ImGuiKey_A;
		locImGuiKeyMapping[Input::KeyB] = ImGuiKey_B;
		locImGuiKeyMapping[Input::KeyC] = ImGuiKey_C;
		locImGuiKeyMapping[Input::KeyD] = ImGuiKey_D;
		locImGuiKeyMapping[Input::KeyE] = ImGuiKey_E;
		locImGuiKeyMapping[Input::KeyF] = ImGuiKey_F;
		locImGuiKeyMapping[Input::KeyG] = ImGuiKey_G;
		locImGuiKeyMapping[Input::KeyH] = ImGuiKey_H;
		locImGuiKeyMapping[Input::KeyI] = ImGuiKey_I;
		locImGuiKeyMapping[Input::KeyJ] = ImGuiKey_J;
		locImGuiKeyMapping[Input::KeyK] = ImGuiKey_K;
		locImGuiKeyMapping[Input::KeyL] = ImGuiKey_L;
		locImGuiKeyMapping[Input::KeyM] = ImGuiKey_M;
		locImGuiKeyMapping[Input::KeyN] = ImGuiKey_N;
		locImGuiKeyMapping[Input::KeyO] = ImGuiKey_O;
		locImGuiKeyMapping[Input::KeyP] = ImGuiKey_P;
		locImGuiKeyMapping[Input::KeyQ] = ImGuiKey_Q;
		locImGuiKeyMapping[Input::KeyR] = ImGuiKey_R;
		locImGuiKeyMapping[Input::KeyS] = ImGuiKey_S;
		locImGuiKeyMapping[Input::KeyT] = ImGuiKey_T;
		locImGuiKeyMapping[Input::KeyU] = ImGuiKey_U;
		locImGuiKeyMapping[Input::KeyV] = ImGuiKey_V;
		locImGuiKeyMapping[Input::KeyW] = ImGuiKey_W;
		locImGuiKeyMapping[Input::KeyX] = ImGuiKey_X;
		locImGuiKeyMapping[Input::KeyY] = ImGuiKey_Y;
		locImGuiKeyMapping[Input::KeyZ] = ImGuiKey_Z;

		locImGuiKeyMapping[Input::KeySpace] = ImGuiKey_Space;
		locImGuiKeyMapping[Input::KeyApostrophe] = ImGuiKey_Apostrophe;
		locImGuiKeyMapping[Input::KeyGraveAccent] = ImGuiKey_GraveAccent;
		locImGuiKeyMapping[Input::KeyComma] = ImGuiKey_Comma;
		locImGuiKeyMapping[Input::KeySemicolon] = ImGuiKey_Semicolon;
		locImGuiKeyMapping[Input::KeyPeriod] = ImGuiKey_Period;
		locImGuiKeyMapping[Input::KeyHyphen] = ImGuiKey_Minus;
		locImGuiKeyMapping[Input::KeyEqual] = ImGuiKey_Equal;
		locImGuiKeyMapping[Input::KeySlash] = ImGuiKey_Slash;
		locImGuiKeyMapping[Input::KeyBackSlash] = ImGuiKey_Backslash;
		locImGuiKeyMapping[Input::KeyLeftBracket] = ImGuiKey_LeftBracket;
		locImGuiKeyMapping[Input::KeyRightBracket] = ImGuiKey_RightBracket;

		locImGuiKeyMapping[Input::KeyEscape] = ImGuiKey_Escape;
		locImGuiKeyMapping[Input::KeyEnter] = ImGuiKey_Enter;
		locImGuiKeyMapping[Input::KeyTab] = ImGuiKey_Tab;
		locImGuiKeyMapping[Input::KeyBackspace] = ImGuiKey_Backspace;
		locImGuiKeyMapping[Input::KeyInsert] = ImGuiKey_Insert;
		locImGuiKeyMapping[Input::KeyDelete] = ImGuiKey_Delete;
		locImGuiKeyMapping[Input::KeyRight] = ImGuiKey_RightArrow;
		locImGuiKeyMapping[Input::KeyLeft] = ImGuiKey_LeftArrow;
		locImGuiKeyMapping[Input::KeyDown] = ImGuiKey_DownArrow;
		locImGuiKeyMapping[Input::KeyUp] = ImGuiKey_UpArrow;
		locImGuiKeyMapping[Input::KeyPageUp] = ImGuiKey_PageUp;
		locImGuiKeyMapping[Input::KeyPageDown] = ImGuiKey_PageDown;
		locImGuiKeyMapping[Input::KeyHome] = ImGuiKey_Home;
		locImGuiKeyMapping[Input::KeyEnd] = ImGuiKey_End;
		locImGuiKeyMapping[Input::KeyCapsLock] = ImGuiKey_CapsLock;
		locImGuiKeyMapping[Input::KeyScrollLock] = ImGuiKey_ScrollLock;
		locImGuiKeyMapping[Input::KeyNumLock] = ImGuiKey_NumLock;
		locImGuiKeyMapping[Input::KeyPrintScreen] = ImGuiKey_PrintScreen;
		locImGuiKeyMapping[Input::KeyPause] = ImGuiKey_Pause;
		locImGuiKeyMapping[Input::KeyF1] = ImGuiKey_F1;
		locImGuiKeyMapping[Input::KeyF2] = ImGuiKey_F2;
		locImGuiKeyMapping[Input::KeyF3] = ImGuiKey_F3;
		locImGuiKeyMapping[Input::KeyF4] = ImGuiKey_F4;
		locImGuiKeyMapping[Input::KeyF5] = ImGuiKey_F5;
		locImGuiKeyMapping[Input::KeyF6] = ImGuiKey_F6;
		locImGuiKeyMapping[Input::KeyF7] = ImGuiKey_F7;
		locImGuiKeyMapping[Input::KeyF8] = ImGuiKey_F8;
		locImGuiKeyMapping[Input::KeyF9] = ImGuiKey_F9;
		locImGuiKeyMapping[Input::KeyF10] = ImGuiKey_F10;
		locImGuiKeyMapping[Input::KeyF11] = ImGuiKey_F11;
		locImGuiKeyMapping[Input::KeyF12] = ImGuiKey_F12;
		locImGuiKeyMapping[Input::KeyLeftShift] = ImGuiMod_Shift;
		locImGuiKeyMapping[Input::KeyLeftCtrl] = ImGuiMod_Ctrl;
		locImGuiKeyMapping[Input::KeyLeftAlt] = ImGuiMod_Alt;
		locImGuiKeyMapping[Input::KeyLeftSuper] = ImGuiMod_Super;
		locImGuiKeyMapping[Input::KeyRightShift] = ImGuiMod_Shift;
		locImGuiKeyMapping[Input::KeyRightCtrl] = ImGuiMod_Ctrl;
		locImGuiKeyMapping[Input::KeyRightAlt] = ImGuiMod_Alt;
		locImGuiKeyMapping[Input::KeyRightSuper] = ImGuiMod_Super;
		locImGuiKeyMapping[Input::KeyMenu] = ImGuiKey_Menu;

		locImGuiKeyMapping[Input::KeyNumPad0] = ImGuiKey_Keypad0;
		locImGuiKeyMapping[Input::KeyNumPad1] = ImGuiKey_Keypad1;
		locImGuiKeyMapping[Input::KeyNumPad2] = ImGuiKey_Keypad2;
		locImGuiKeyMapping[Input::KeyNumPad3] = ImGuiKey_Keypad3;
		locImGuiKeyMapping[Input::KeyNumPad4] = ImGuiKey_Keypad4;
		locImGuiKeyMapping[Input::KeyNumPad5] = ImGuiKey_Keypad5;
		locImGuiKeyMapping[Input::KeyNumPad6] = ImGuiKey_Keypad6;
		locImGuiKeyMapping[Input::KeyNumPad7] = ImGuiKey_Keypad7;
		locImGuiKeyMapping[Input::KeyNumPad8] = ImGuiKey_Keypad8;
		locImGuiKeyMapping[Input::KeyNumPad9] = ImGuiKey_Keypad9;
		locImGuiKeyMapping[Input::KeyNumPadDecimal] = ImGuiKey_KeypadDecimal;
		locImGuiKeyMapping[Input::KeyNumPadDivide] = ImGuiKey_KeypadDivide;
		locImGuiKeyMapping[Input::KeyNumPadMultiply] = ImGuiKey_KeypadMultiply;
		locImGuiKeyMapping[Input::KeyNumPadSubtract] = ImGuiKey_KeypadSubtract;
		locImGuiKeyMapping[Input::KeyNumPadAdd] = ImGuiKey_KeypadAdd;
		locImGuiKeyMapping[Input::KeyNumPadEnter] = ImGuiKey_KeypadEnter;
		locImGuiKeyMapping[Input::KeyNumPadEqual] = ImGuiKey_KeypadEqual;
	}

	Gui::Gui(GLFWwindow* aWindow, bool aMenuBar)
		: myWindow(aWindow)
		, myHasMenuBar(aMenuBar)
	{
		locInitImGuiMapping();

		myGuiContext = ImGui::CreateContext();
		myPlotContext = ImPlot::CreateContext();

		glfwGetWindowSize(myWindow, &myWindowWidth, &myWindowHeight);
		myWindowResizeCallbackId = Core::WindowModule::GetInstance()->AddWindowSizeCallback([this](int aWidth, int aHeight) {
			myWindowWidth = aWidth;
			myWindowHeight = aHeight;
		}, myWindow);

		glfwGetWindowContentScale(myWindow, &myContentScaleX, &myContentScaleY);
		myWindowContentScaleCallbackId = Core::WindowModule::GetInstance()->AddContentScaleCallback([this](float aContentScaleX, float aContentScaleY) {
			myContentScaleX = aContentScaleX;
			myContentScaleY = aContentScaleY;
			// Not sure why we don't need to update style.FontScaleDpi here?..
		}, myWindow);

		myMouseCallbackId = Core::InputModule::GetInstance()->AddMouseCallback([this](Input::MouseButton aButton, Input::Status aStatus, Input::Modifier someModifiers) {
			(void)someModifiers;
			if (aStatus == Input::Status::Pressed)
				myMouseData.push({ locImGuiMouseMapping[aButton], true });
			else if (aStatus == Input::Status::Released)
				myMouseData.push({ locImGuiMouseMapping[aButton], false });
		}, myWindow);

		myCursorPosCallbackId = Core::InputModule::GetInstance()->AddCursorCallback([this](double aXPos, double aYPos) {
			myCursorMoved = true;
			myCursorXPos = aXPos;
			myCursorYPos = aYPos;
		}, myWindow);

		myTouchCallbackId = Core::InputModule::GetInstance()->AddTouchCallback([this](uint64 aFingerId, double anX, double anY, int anUp) {
			bool isFakeTouch = Core::InputModule::GetInstance()->IsFakeTouchId(aFingerId);
			if (anUp)
			{
				if (!isFakeTouch && myTouchToMouseFingerId.has_value() && myTouchToMouseFingerId.value() == aFingerId)
				{
					myTouchToMouseEventsQueue.push({ false, { anX, anY } });
					myTouchToMouseFingerId.reset();
				}
				myActiveTouches.erase(aFingerId);
			}
			else
			{
				std::optional<uint64> previousTouchToMouseFingerId = myTouchToMouseFingerId;

				auto previousTouch = myActiveTouches.find(aFingerId);
				if (previousTouch != myActiveTouches.end())
				{
					// Early return if duplicated touch event
					if (std::abs(anX - previousTouch->second.x) < DBL_EPSILON && std::abs(anY - previousTouch->second.y) < DBL_EPSILON)
						return;

					// If we don't have a touch to mouse yet, this finger id becomes our new one
					if (!isFakeTouch && !myTouchToMouseFingerId.has_value())
						myTouchToMouseFingerId = aFingerId;
				}
				else if (!isFakeTouch)
				{
					// New finger id, this is our new touch to mouse
					myTouchToMouseFingerId = aFingerId;
				}

				// If the touch to mouse finger id changed, send a mouse up event for the previous one
				if (previousTouchToMouseFingerId.has_value() && previousTouchToMouseFingerId != myTouchToMouseFingerId)
				{
					previousTouch = myActiveTouches.find(previousTouchToMouseFingerId.value());
					if (previousTouch != myActiveTouches.end())
					{
						myTouchToMouseEventsQueue.push({ false, previousTouch->second });
					}
				}

				if (myTouchToMouseFingerId.has_value() && myTouchToMouseFingerId.value() == aFingerId)
				{
					myTouchToMouseEventsQueue.push({ true, { anX, anY } });
				}

				myActiveTouches[aFingerId] = { anX, anY };
			}
		}, myWindow);

		myScrollCallbackId = Core::InputModule::GetInstance()->AddScrollCallback([this](double aXScroll, double aYScroll) {
			myScrollChanged = true;
			myXScroll += aXScroll;
			myYScroll += aYScroll;
		}, myWindow);

		myKeyCallbackId = Core::InputModule::GetInstance()->AddKeyCallback([this](Input::Key aKey, Input::Status aStatus, Input::Modifier someModifiers) {
			(void)someModifiers;
			if (aStatus == Input::Status::Pressed)
				myKeyData.push({ locImGuiKeyMapping[aKey], true });
			else if (aStatus == Input::Status::Released)
				myKeyData.push({ locImGuiKeyMapping[aKey], false });
		}, myWindow);

		myCharacterCallbackId = Core::InputModule::GetInstance()->AddCharacterCallback([this](uint aUnicodeCodePoint) {
			myTextInput.push(aUnicodeCodePoint);
		}, myWindow);

		ImGui::SetCurrentContext(myGuiContext);
		InitIO();
		InitStyle();
		PrepareFont();
	}

	Gui::~Gui()
	{
		Core::WindowModule::GetInstance()->RemoveWindowSizeCallback(myWindowResizeCallbackId);
		Core::InputModule::GetInstance()->RemoveMouseCallback(myMouseCallbackId);
		Core::InputModule::GetInstance()->RemoveCursorCallback(myCursorPosCallbackId);
		Core::InputModule::GetInstance()->RemoveTouchCallback(myTouchCallbackId);
		Core::InputModule::GetInstance()->RemoveScrollCallback(myScrollCallbackId);
		Core::InputModule::GetInstance()->RemoveKeyCallback(myKeyCallbackId);
		Core::InputModule::GetInstance()->RemoveCharacterCallback(myCharacterCallbackId);

		ImPlot::DestroyContext(myPlotContext);
		ImGui::DestroyContext(myGuiContext);
	}

	void Gui::Update(const std::function<void()>& aCallback)
	{
		ImGui::SetCurrentContext(myGuiContext);
		ImPlot::SetCurrentContext(myPlotContext);

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)myWindowWidth, (float)myWindowHeight);
		io.DeltaTime = Core::TimeModule::GetInstance()->GetDeltaTimeSec();

		while (!myMouseData.empty())
		{
			MouseData& data = myMouseData.front();
			io.AddMouseButtonEvent(data.myButton, data.myDown);
			myMouseData.pop();
		}

		if (myCursorMoved)
		{
			io.AddMousePosEvent((float)myCursorXPos, (float)myCursorYPos);
			myCursorMoved = false;
			myCursorXPos = myCursorYPos = 0.0;
		}

		while (!myTouchToMouseEventsQueue.empty())
		{
			TouchToMouseEvent& event = myTouchToMouseEventsQueue.front();
			io.AddMousePosEvent(event.myPos.x, event.myPos.y);
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, event.myDown);
			myTouchToMouseEventsQueue.pop();
		}

		if (myScrollChanged)
		{
			io.AddMouseWheelEvent((float)myXScroll, (float)myYScroll);
			myScrollChanged = false;
			myXScroll = myYScroll = 0.0;
		}

		while (!myKeyData.empty())
		{
			KeyData& data = myKeyData.front();
			io.AddKeyEvent(ImGuiKey(data.myKey), data.myDown);
			myKeyData.pop();
		}

		while (!myTextInput.empty())
		{
			io.AddInputCharacter(myTextInput.front());
			myTextInput.pop();
		}

		ImGui::NewFrame();

		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGuiWindowFlags mainFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
			if (myHasMenuBar)
				mainFlags |= ImGuiWindowFlags_MenuBar;

			if (ImGui::Begin("main", nullptr, mainFlags))
			{
				if (aCallback)
					aCallback();
			}
			ImGui::End();

			ImGui::PopStyleVar();
		}

		ImGui::Render();

		ImDrawData* imDrawData = ImGui::GetDrawData();
		if (!imDrawData || imDrawData->TotalVtxCount <= 0)
			return;

		VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		if (vertexBufferSize == 0 || indexBufferSize == 0)
			return;

		// Vertex buffer
		if (!myVertexBuffer || myVertexCount != imDrawData->TotalVtxCount)
		{
			myVertexBuffer = new Buffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			myVertexCount = imDrawData->TotalVtxCount;
			myVertexBuffer->Map();
		}

		// Index buffer
		if (!myIndexBuffer || myIndexCount != imDrawData->TotalIdxCount)
		{
			myIndexBuffer = new Buffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			myIndexCount = imDrawData->TotalIdxCount;
			myIndexBuffer->Map();
		}

		// Update data
		ImDrawVert* vtxDst = (ImDrawVert*)myVertexBuffer->myMappedData;
		ImDrawIdx* idxDst = (ImDrawIdx*)myIndexBuffer->myMappedData;
		for (int i = 0; i < imDrawData->CmdListsCount; ++i)
		{
			const ImDrawList* cmdList = imDrawData->CmdLists[i];
			memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmdList->VtxBuffer.Size;
			idxDst += cmdList->IdxBuffer.Size;
		}

		myVertexBuffer->Flush();
		myIndexBuffer->Flush();
	}

	void Gui::Draw(VkCommandBuffer aCommandBuffer, VkPipelineLayout aPipelineLayout, uint aDescriptorSetIndex, const std::function<VkDescriptorSet(const ShaderHelpers::DescriptorInfo&)>& myDescriptorSetGetter)
	{
		ImGui::SetCurrentContext(myGuiContext);

		ImDrawData* imDrawData = ImGui::GetDrawData();
		if (!imDrawData || imDrawData->TotalVtxCount <= 0)
			return;

		if (imDrawData->Textures)
		{
			for (ImTextureData* texture : *imDrawData->Textures)
			{
				UpdateTexture(texture);
			}
		}

		ImGuiIO& io = ImGui::GetIO();
		myPushConstBlock.myScale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
		myPushConstBlock.myTranslate = glm::vec2(-1.0f);
		vkCmdPushConstants(aCommandBuffer, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(myPushConstBlock), &myPushConstBlock);

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, &myVertexBuffer->myBuffer, offsets);
		vkCmdBindIndexBuffer(aCommandBuffer, myIndexBuffer->myBuffer, 0, VK_INDEX_TYPE_UINT32); // See ImDrawIdx declaration

		int vertexOffset = 0;
		int indexOffset = 0;
		for (const ImDrawList* cmdList : imDrawData->CmdLists)
		{
			for (const ImDrawCmd& cmd : cmdList->CmdBuffer)
			{
				VkRect2D scissorRect{};
				scissorRect.offset.x = std::max((int)cmd.ClipRect.x, 0);
				scissorRect.offset.y = std::max((int)cmd.ClipRect.y, 0);
				scissorRect.extent.width = (uint32_t)(cmd.ClipRect.z - cmd.ClipRect.x);
				scissorRect.extent.height = (uint32_t)(cmd.ClipRect.w - cmd.ClipRect.y);
				vkCmdSetScissor(aCommandBuffer, 0, 1, &scissorRect);

				ImTextureID textureID = cmd.GetTexID();
				if (textureID != ImTextureID_Invalid)
				{
					ShaderHelpers::GuiDescriptorInfo info;
					info.myFontSamplerInfo = (const VkDescriptorImageInfo*)textureID;
					VkDescriptorSet descriptorSet = myDescriptorSetGetter(info);
					vkCmdBindDescriptorSets(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, aDescriptorSetIndex, 1, &descriptorSet, 0, nullptr);
				}

				vkCmdDrawIndexed(aCommandBuffer, cmd.ElemCount, 1, indexOffset, vertexOffset, 0);
				indexOffset += cmd.ElemCount;
			}
			vertexOffset += cmdList->VtxBuffer.Size;
		}
	}

	void Gui::InitIO()
	{
		ImGuiIO& io = ImGui::GetIO();

		// For now, disable the .ini files, as it is not useful so far
		io.IniFilename = nullptr;

		// We are now responsible to create/update/destroy the textures as requested by ImGui
		io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
	}

	void Gui::InitStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.FontScaleDpi = myContentScaleY;

		// For now keep the defaults
		//style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		//style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		//style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
		//style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
		//style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		//style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		//style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		//style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
		//style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		//style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
		//style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	}

	void Gui::PrepareFont()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();
		myFontMap.Clear();
		myFontMap.SetFont(FontType::Regular, io.Fonts->AddFontFromFileTTF(FileHelpers::RedirectFilePath("Modules/Fonts/NotoSans-Regular.ttf").c_str(), 0.f), 16.f);
		myFontMap.SetFont(FontType::Bold, io.Fonts->AddFontFromFileTTF(FileHelpers::RedirectFilePath("Modules/Fonts/NotoSans-Bold.ttf").c_str(), 0.f), 16.f);
		myFontMap.SetFont(FontType::Italic, io.Fonts->AddFontFromFileTTF(FileHelpers::RedirectFilePath("Modules/Fonts/NotoSans-Italic.ttf").c_str(), 0.f), 16.f);
		myFontMap.SetFont(FontType::Large, io.Fonts->AddFontFromFileTTF(FileHelpers::RedirectFilePath("Modules/Fonts/NotoSans-Regular.ttf").c_str(), 0.f), 32.f);
		myFontMap.SetFont(FontType::Title, io.Fonts->AddFontFromFileTTF(FileHelpers::RedirectFilePath("Modules/Fonts/NotoSans-Bold.ttf").c_str(), 0.f), 32.f);
		io.Fonts->CompactCache();
	}

	void Gui::UpdateTexture(ImTextureData* aTexture)
	{
		if (!aTexture || aTexture->Status == ImTextureStatus_OK)
			return;

		if (aTexture->Status == ImTextureStatus_WantCreate)
		{
			Assert(!myTextures.contains(aTexture->UniqueID), "New ImGui texture already in the textures map when creating!");

			ImagePtr& newTexture = myTextures[aTexture->UniqueID];
			newTexture = new Image(aTexture->Width, aTexture->Height,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			newTexture->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
			newTexture->CreateImageSampler();
			newTexture->SetupDescriptor();

			aTexture->SetTexID((ImTextureID)&newTexture->myDescriptor);
		}

		if (aTexture->Status == ImTextureStatus_WantCreate || aTexture->Status == ImTextureStatus_WantUpdates)
		{
			Assert(myTextures.contains(aTexture->UniqueID), "Existing ImGui texture not in the textures map when updating!");
			ImagePtr& existingTexture = myTextures[aTexture->UniqueID];

			VkDeviceSize textureSize = aTexture->Width * aTexture->Height * 4;

			Buffer textureStaging;
			textureStaging.Create(textureSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			textureStaging.Map();
			memcpy(textureStaging.myMappedData, aTexture->Pixels, static_cast<size_t>(textureSize));
			textureStaging.Unmap();

			existingTexture->TransitionLayout(VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				RenderCore::GetInstance()->GetGraphicsQueue());

			VkCommandBuffer commandBuffer = Helpers::BeginOneTimeCommand();
			{
				VkBufferImageCopy imageCopyRegion{};
				imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageCopyRegion.imageSubresource.mipLevel = 0;
				imageCopyRegion.imageSubresource.baseArrayLayer = 0;
				imageCopyRegion.imageSubresource.layerCount = 1;
				imageCopyRegion.imageExtent = { static_cast<uint>(aTexture->Width), static_cast<uint>(aTexture->Height), 1 };
				vkCmdCopyBufferToImage(commandBuffer, textureStaging.myBuffer, existingTexture->myImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
			}
			Helpers::EndOneTimeCommand(commandBuffer, RenderCore::GetInstance()->GetGraphicsQueue());

			textureStaging.Destroy();

			existingTexture->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				RenderCore::GetInstance()->GetGraphicsQueue());

			aTexture->SetStatus(ImTextureStatus_OK);
		}

		if (aTexture->Status == ImTextureStatus_WantDestroy)
		{
			Assert(myTextures.contains(aTexture->UniqueID), "Existing ImGui texture not in the textures map when destroying!");
			myTextures.erase(aTexture->UniqueID);

			aTexture->SetTexID(ImTextureID_Invalid);
			aTexture->SetStatus(ImTextureStatus_Destroyed);
		}
	}
}
