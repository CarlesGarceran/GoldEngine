#pragma once

namespace Engine::Render::BuiltIn
{
	public ref class ACES_Tonemapper : Engine::Render::ScriptableEffect
	{
	private:
		Engine::Native::EnginePtr<RAYLIB::Shader>* tonemapper;

	public:
		ACES_Tonemapper();

	public:
		bool ManualRendering() override;

		void OnEffectBegin() override;
		void OnEffectEnd() override;
		void OnEffectUnload() override;

		void SetTexture(RAYLIB::Texture* texturePtr) override;
	};
}