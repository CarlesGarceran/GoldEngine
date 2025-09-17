#include "../Includes.h"
#include <vector>
#include <string>
#include <chrono>
#include "ImNotifyWrapper.h"
#include <gcroot.h>

std::function<void()> ConvertAction(System::Action^ action)
{
	// Create a gcroot to hold the managed delegate inside unmanaged code
	msclr::gcroot<System::Action^> managedAction(action);

	return [managedAction]() {
		managedAction->Invoke();
	};
}


inline ImGuiToast toToast(ImGuiNET::ImNotify::ImNotification notification)
{
	ImGuiToast toast((ImGuiToastType_)notification.type, notification.dismissTime);
	toast.setTitle(CastStringToNative(notification.title).c_str());
	toast.setContent(CastStringToNative(notification.content).c_str());

	return toast;
}

void ImGuiNET::ImNotify::ImNotify::InsertNotification(ImGuiNET::ImNotify::ImNotification notification)
{
	ImGuiToast toast = toToast(notification);
	toast.setOnButtonPress(ConvertAction(notification.buttonClicked));

	::ImGui::InsertNotification(toast);
}

void ImGuiNET::ImNotify::ImNotify::RemoveNotification(int index)
{
	::ImGui::RemoveNotification(index);
}

void ImGuiNET::ImNotify::ImNotify::RenderNotifications(System::Numerics::Vector2 position, System::Numerics::Vector2 size)
{
	::ImGui::RenderNotifications(
		{
			position.X,
			position.Y
		},
		{
			size.X,
			size.Y
		}
	);
}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type)
	: ImGuiNET::ImNotify::ImNotification::ImNotification(type, NOTIFY_DEFAULT_DISMISS) {}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time)
	: ImGuiNET::ImNotify::ImNotification::ImNotification(type, NOTIFY_DEFAULT_DISMISS, "") {}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents)
	: ImGuiNET::ImNotify::ImNotification::ImNotification(type, NOTIFY_DEFAULT_DISMISS, contents, "") {}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title)
	: ImGuiNET::ImNotify::ImNotification::ImNotification(type, NOTIFY_DEFAULT_DISMISS, contents, title, "") {}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title, System::String^ buttonText)
	: ImGuiNET::ImNotify::ImNotification::ImNotification(type, NOTIFY_DEFAULT_DISMISS, contents, title, buttonText, nullptr) {}

ImGuiNET::ImNotify::ImNotification::ImNotification(ImGuiNET::ImNotify::ImGuiToastType type, int dismiss_time, System::String^ contents, System::String^ title, System::String^ buttonText, System::Action^ buttonAction)
{
	this->type = type;
	this->title = "";
	this->content = contents;
	this->dismissTime = dismiss_time;
	this->buttonText = buttonText;
	this->buttonClicked = buttonAction;
}

void ImGuiNET::ImNotify::ImNotification::OnClicked()
{
	this->buttonClicked->Invoke();
}


