#pragma once

namespace Engine::EngineObjects
{
	public ref class BoundingBoxRenderer : public Engine::Internal::Components::GameObject
	{
	public:
		unsigned long color;

	public:
		BoundingBoxRenderer(String^ name, Engine::Internal::Components::Transform^ trans, unsigned long tint) : Engine::Internal::Components::GameObject(name, trans, Engine::Internal::Components::ObjectType::BoundingBoxRenderer, this->tag, Engine::Scripting::LayerManager::GetLayerFromId(1))
		{
			this->color = tint;
		}

		void Init(unsigned long tint) override
		{
			this->color = tint;
		}

		void Draw() override
		{
			auto v3 = getTransform()->scale.toNative();
			RAYLIB::DrawCubeWires(getTransform()->position.toNative(), (float)v3.x, (float)v3.y, (float)v3.z, GetColor(color));
		}
	};
}