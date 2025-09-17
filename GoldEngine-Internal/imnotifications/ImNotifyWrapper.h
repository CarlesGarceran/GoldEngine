#pragma once

namespace ImGuiNET::ImNotify
{
	public enum class ImGuiToastType
	{
		ImGuiToastType_None,
		ImGuiToastType_Success,
		ImGuiToastType_Warning,
		ImGuiToastType_Error,
		ImGuiToastType_Info,
		ImGuiToastType_COUNT
	};

	public enum class ImGuiToastPhase
	{
		ImGuiToastPhase_FadeIn,
		ImGuiToastPhase_Wait,
		ImGuiToastPhase_FadeOut,
		ImGuiToastPhase_Expired,
		ImGuiToastPhase_COUNT
	};

	public enum class ImGuiToastPos
	{
		ImGuiToastPos_TopLeft,
		ImGuiToastPos_TopCenter,
		ImGuiToastPos_TopRight,
		ImGuiToastPos_BottomLeft,
		ImGuiToastPos_BottomCenter,
		ImGuiToastPos_BottomRight,
		ImGuiToastPos_Center,
		ImGuiToastPos_COUNT
	};

	[Engine::Attributes::LuaAPIAttribute]
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public value struct ImNotification
	{
	public:
		ImGuiNET::ImNotify::ImGuiToastType type;
		System::String^ title;
		System::String^ content;
		int dismissTime;

		System::String^ buttonText;
		System::Action^ buttonClicked;

	public:
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type);
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time);
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents);
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title);
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title, System::String^ buttonText);
		ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title, System::String^ buttonText, System::Action^ buttonAction);
	
	private:
		void OnClicked();
	};

	[Engine::Attributes::LuaAPIAttribute]
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class ImNotify
	{
	public:
		static void InsertNotification(ImNotification notification);
		static void RemoveNotification(int index);


		static void RenderNotifications(System::Numerics::Vector2 position, System::Numerics::Vector2 size);
	};
}