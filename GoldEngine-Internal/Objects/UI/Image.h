#pragma once

#include "../../InputManager.h"

namespace Engine::EngineObjects::UI
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Image : Engine::EngineObjects::ScriptBehaviour
	{
	public:
		Image(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
			if (!attributes->hasAttribute("Texture ID"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Texture ID", gcnew System::UInt32(0), System::UInt32::typeid));

			if (!attributes->getAttribute("Image Tint"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Image Tint", gcnew Engine::Components::Color(0xFFFFFFFF)));

			if (!attributes->getAttribute("Image Rotation"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Image Rotation", 0.0f, System::Single::typeid));
		}

	public:
		void Start() override
		{
			if (!attributes->hasAttribute("Texture ID"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Texture ID", gcnew System::UInt32(0), System::UInt32::typeid));

			if (!attributes->getAttribute("Image Tint"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Image Tint", gcnew Engine::Components::Color(0xFFFFFFFF)));

			if (!attributes->getAttribute("Image Rotation"))
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Image Rotation", 0.0f, System::Single::typeid));
		}

	public:
		void DrawGUI() override
		{
			RAYLIB::BeginBlendMode(RAYLIB::BlendMode::BLEND_ALPHA);

			Engine::Components::Vector2 v2 = this->transform->position.toVector2();
			RAYLIB::Texture texture = Engine::Assets::Storage::DataPacks::DataPacks::singleton().GetTexture2D((unsigned int)attributes->getAttribute("Texture ID")->getValueAuto());

			RAYLIB::Rectangle outRectangle;

			outRectangle.x = 0;
			outRectangle.y = 0;
			outRectangle.width = this->transform->scale.x;
			outRectangle.height = this->transform->scale.y;

			RAYLIB::Rectangle inRectangle;

			inRectangle.x = 0;
			inRectangle.y = 0;
			inRectangle.width = texture.width;
			inRectangle.height = texture.height;

			DrawTexturePro(
				texture,
				inRectangle,
				outRectangle,
				v2.toNative(),
				attributes->getAttribute("Image Rotation")->getValue<float>(),
				attributes->getAttribute("Image Tint")->getValue<Engine::Components::Color^>()->toNative()
			);

			
			RAYLIB::EndBlendMode();
		}
	};
}