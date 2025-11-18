#pragma once

namespace Engine::EngineObjects
{
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
			Engine::Components::Vector2 transformedVector = this->transform->position.toVector2();
			RAYLIB::Texture texture = Engine::Assets::Storage::DataPacks::singleton().GetTexture2D(TextureId);

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