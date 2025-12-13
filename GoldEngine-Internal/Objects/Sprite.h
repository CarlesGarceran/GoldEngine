#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Sprite : Engine::EngineObjects::ScriptBehaviour
	{
	public:
		[Engine::Scripting::PropertyAttribute("Texture ID")] unsigned int TextureId = 0;
		[Engine::Scripting::PropertyAttribute("Sprite Tint")] Engine::Components::Color^ SpriteTint = gcnew Engine::Components::Color(0xFFFFFFFF);

	public:
		Sprite(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
		}

	public:
		void Start() override
		{

		}

		void Update() override
		{

		}

		void Draw() override
		{
			auto pos = this->transform->position.toVector2();
			auto scale = this->transform->scale;

			RAYLIB::Texture texture = Engine::Assets::Storage::DataPacks::singleton().GetTexture2D(TextureId);

			RAYLIB::Rectangle source = { 0, 0, -(float)texture.width, -(float)texture.height };

			RAYLIB::Rectangle dest;
			dest.width = scale.x;
			dest.height = scale.y;
			dest.x = pos.x - dest.width * 0.5f;
			dest.y = pos.y - dest.height * 0.5f;

			RAYLIB::Vector2 origin = { dest.width * 0.5f, dest.height * 0.5f };

			DrawTexturePro(
				texture,
				source,
				dest,
				origin,
				transform->rotation.y,
				SpriteTint->toNative()
			);
		}

		void Destroy() override
		{
			delete this;
		}
	};
}