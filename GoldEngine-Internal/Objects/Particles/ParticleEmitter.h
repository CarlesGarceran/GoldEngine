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
		[Engine::Scripting::PropertyAttribute] Enums::EmissionDirection emissionDirection;

		[Engine::Scripting::PropertyAttribute] bool emitting;

		[Engine::Scripting::PropertyAttribute] Enums::EmitterShape shape;
		[Engine::Scripting::PropertyAttribute] Enums::EmitterShapeInOut shapeInOut;
		[Engine::Scripting::PropertyAttribute] Enums::EmitterShapeStyle shapeStyle;

	public:
		virtual void Start() override;
		virtual void Update() override;
		virtual void Draw() override = 0;
	};
}