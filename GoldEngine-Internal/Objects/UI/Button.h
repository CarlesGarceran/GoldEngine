#pragma once

#include "../../Screen.h"
#include "../../InputManager.h"

namespace Engine::EngineObjects::UI
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Button : Engine::EngineObjects::ScriptBehaviour
	{
	public:
			[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onMouseButton1Down;
			[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onMouseButton1Click;
			[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onMouseButton2Down;
			[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Scripting::Events::Event^ onMouseButton2Click;

	public:
		Button(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
			onMouseButton1Down = gcnew Engine::Scripting::Events::Event();
			onMouseButton1Click = gcnew Engine::Scripting::Events::Event();
			onMouseButton2Down = gcnew Engine::Scripting::Events::Event();
			onMouseButton2Click = gcnew Engine::Scripting::Events::Event();
		}

	public:
		void Start() override
		{
			if (!attributes->hasAttribute("button_color"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_color", gcnew Engine::Components::Color(0xFFFFFFFF), Engine::Components::Color::typeid));

			if (!attributes->hasAttribute("button_text"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text", "Button Text", String::typeid));

			if (!attributes->hasAttribute("button_text_size"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text_size", gcnew Int32(12), Int32::typeid));

			if (!attributes->hasAttribute("button_text_color"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text_color", gcnew Engine::Components::Color(0x000000FF), Engine::Components::Color::typeid));

			if (!attributes->hasAttribute("button_text_spacing"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text_spacing", 0.0f, Single::typeid));

			if (!attributes->hasAttribute("button_text_rotation"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text_rotation", 0.0f, Single::typeid));

			if (!attributes->hasAttribute("button_text_origin"))
				attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "button_text_origin", gcnew Engine::Components::Vector2(0,0), Engine::Components::Vector2::typeid));
		}

	public:
		void DrawGUI() override
		{
			RAYLIB::BeginBlendMode(RAYLIB::BlendMode::BLEND_ALPHA);

			auto posV2 = this->transform->position.toVector2();
			auto sizV2 = this->transform->scale.toVector2();
			auto color = attributes->getAttribute("button_color")->getValueAs<Engine::Components::Color^>();
			auto text = CastStringToNative(attributes->getAttribute("button_text")->getValueAs<String^>());
			auto textSize = (int)attributes->getAttribute("button_text_size")->getValueAs<int>();
			auto textColor = attributes->getAttribute("button_text_color")->getValueAs<Engine::Components::Color^>();
			auto textSpacing = (float)attributes->getAttribute("button_text_spacing")->getValueAs<float>();
			auto textRotation = (float)attributes->getAttribute("button_text_rotation")->getValueAs<float>();
			auto textOrigin = attributes->getAttribute("button_text_origin")->getValueAs<Engine::Components::Vector2>();
			RAYLIB::Font font = RAYLIB::GetFontDefault();
			RAYLIB::Vector2 positionV2 = posV2.toNative();

			RAYLIB::Rectangle rectangle;

			rectangle.x = positionV2.x;
			rectangle.y = positionV2.y;
			rectangle.width = sizV2.x;
			rectangle.height = sizV2.y;

			DrawRectangleRec(rectangle, color->toNative());
			DrawTextPro(font, text.c_str(), positionV2, textOrigin.toNative(), textRotation, textSize, textSpacing, textColor->toNative());

			RAYLIB::EndBlendMode();
		}

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override
		{
			auto mousePosition = Engine::Scripting::InputManager::GetMousePosition();
			auto posV2 = this->transform->position.toVector2();
			auto sizV2 = this->transform->scale.toVector2();

			RAYLIB::Rectangle rectangle;

			rectangle.x = posV2.x;
			rectangle.y = posV2.y;
			rectangle.width = sizV2.x;
			rectangle.height = sizV2.y;
			
			if (RAYLIB::CheckCollisionPointRec(mousePosition.toNative(), rectangle))
			{
				if (RAYLIB::IsMouseButtonDown(RAYLIB::MOUSE_BUTTON_LEFT))
					onMouseButton1Down->raiseExecution();

				if (RAYLIB::IsMouseButtonPressed(RAYLIB::MOUSE_BUTTON_LEFT))
					onMouseButton1Click->raiseExecution();

				if (RAYLIB::IsMouseButtonDown(RAYLIB::MOUSE_BUTTON_RIGHT))
					onMouseButton2Down->raiseExecution();

				if (RAYLIB::IsMouseButtonPressed(RAYLIB::MOUSE_BUTTON_RIGHT))
					onMouseButton2Click->raiseExecution();
			}
		}

	};
}