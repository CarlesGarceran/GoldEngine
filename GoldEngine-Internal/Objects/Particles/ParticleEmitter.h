#pragma once

namespace Engine::EngineObjects::Particles
{
	namespace Enums
	{
		public enum class EmissionDirection 
		{
			Back,
			Bottom,
			Front,
			Left,
			Right,
			Top
		};

		public enum class EmitterShape
		{
			Box,
			Cylinder,
			Mesh,
			Disc,
			Sphere
		};

		public enum class EmitterShapeInOut
		{
			InAndOut,
			Outward,
			Inward
		};

		public enum class EmitterShapeStyle
		{
			Surface,
			Volume
		};
	};

	public ref class ParticleEmitter abstract : public Engine::EngineObjects::Script
	{
	public:
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] Enums::EmissionDirection emissionDirection;

		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] bool emitting;

		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] Enums::EmitterShape shape;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] Enums::EmitterShapeInOut shapeInOut;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] Enums::EmitterShapeStyle shapeStyle;

	public:
		ParticleEmitter();
		ParticleEmitter(String^ name, Engine::Internal::Components::Transform^ transform);
	
		virtual void Start() override;
		virtual void Update() override;
		virtual void Draw() override = 0;
	};
}