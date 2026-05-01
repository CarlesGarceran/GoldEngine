#pragma once

namespace Engine::EngineObjects::Geometry::Abstract
{
	public ref class Renderer abstract : public Engine::EngineObjects::Script
	{
	public:
		virtual RAYLIB::Model& GetModel() = 0;
		virtual RAYLIB::Model* GetModelPtr() = 0;
	};
}