#pragma once

namespace Engine::EngineObjects
{
	public ref class Sprite : Engine::EngineObjects::ScriptBehaviour
	{
	public:
		Sprite(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
			if (!attributes->getAttribute("Texture ID"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Texture ID", gcnew System::UInt32(0), System::UInt32::typeid));
			}

			if (!attributes->getAttribute("Sprite Tint"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Sprite Tint", gcnew Engine::Components::Color(0xFFFFFFFF)));
			}

			if (!attributes->getAttribute("Sprite Rotation"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Sprite Rotation", 0.0f, System::Single::typeid));
			}
		}

	public:
		void Start() override
		{
			if (!attributes->getAttribute("Texture ID"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Texture ID", gcnew System::UInt32(0), System::UInt32::typeid));
			}

			if (!attributes->getAttribute("Sprite Tint"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Sprite Tint", gcnew Engine::Components::Color(0xFFFFFFFF)));
			}

			if (!attributes->getAttribute("Sprite Rotation"))
			{
				attributes->addAttribute(Engine::Scripting::Attribute::create(Engine::Scripting::AccessLevel::Public, "Sprite Rotation", 0.0f, System::Single::typeid));
			}
		}

		void Update() override
		{

		}

		void Draw() override
		{
			Engine::Components::Vector2 transformedVector = this->transform->position.toVector2();
			RAYLIB::Texture texture = Engine::Assets::Storage::DataPacks::singleton().GetTexture2D((unsigned int)attributes->getAttribute("Texture ID")->getValueAuto());

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
				transformedVector.toNative(),
				attributes->getAttribute("Sprite Rotation")->getValue<float>(),
				attributes->getAttribute("Sprite Tint")->getValue<Engine::Components::Color^>()->toNative()
			);
		}

		void Destroy() override
		{
			delete this;
		}
	};

}