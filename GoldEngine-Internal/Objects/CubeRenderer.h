#pragma once

namespace Engine::EngineObjects
{
	public ref class CubeRenderer : public Engine::Internal::Components::GameObject
	{
	public:
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ color;

	public:
		CubeRenderer(String^ name, Engine::Internal::Components::Transform^ trans, unsigned int tint) : Engine::Internal::Components::GameObject(name, trans, Engine::Internal::Components::ObjectType::CubeRenderer, this->tag, Engine::Scripting::LayerManager::GetLayerFromId(1))
		{
			this->color = gcnew Engine::Components::Color(tint);
		}

		void Init(unsigned int tint) override
		{
			this->color = gcnew Engine::Components::Color(tint);
		}

		void Draw() override
		{
			auto v3 = getTransform()->scale.toNative();
			DrawCube(getTransform()->position.toNative(), v3.x, v3.y, v3.z , color->toNative());
		}
	};
}