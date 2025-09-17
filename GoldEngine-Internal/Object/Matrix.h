#pragma once

namespace Engine::Components
{
	namespace Abstract
	{
		public ref class Mat abstract
		{
		public:
			initonly int size;

			virtual property int Rows;
			virtual property int Columns;

			virtual array<float>^ GetValues() = 0;
			virtual void SetValues(array<float>^ values) = 0;

		protected:
			Mat(int size)
			{
				this->size = size;
			}
		};
	}

	public ref class Matrix4 : Engine::Components::Abstract::Mat
	{
	public:
		float m0, m1;
		float m2, m3;

		Matrix4();

		virtual property int Rows { int get() override { return 2; } }
		virtual property int Columns { int get() override { return 2; } }

		array<float>^ GetValues() override;
		void SetValues(array<float>^ values) override;
	};

	public ref class Matrix6 : Engine::Components::Abstract::Mat
	{
	public:
		float m0, m1, m2;
		float m3, m4, m5;

		Matrix6();
		Matrix6(System::Numerics::Matrix3x2 matrix);

		array<float>^ GetValues() override;
		void SetValues(array<float>^ values) override;
	};

	public ref class Matrix16 : Engine::Components::Abstract::Mat
	{
	public:
		float m0,	m1,		m2,		m3;
		float m4,	m5,		m6,		m7;
		float m8,	m9,		m10,	m11;
		float m12,	m13,	m14,	m15;

	public:
		Matrix16();
		Matrix16(RAYLIB::Matrix matrix);
		Matrix16(System::Numerics::Matrix4x4 matrix);

		array<float>^ GetValues() override;
		void SetValues(array<float>^ values) override;

		static Matrix16^ MatrixMultiply(Matrix16^ left, Matrix16^ right);

		RAYLIB::Matrix toNative();
	};

	generic <class M>
	where M : Engine::Components::Abstract::Mat, gcnew()
	public ref class Matrix
	{
	public:
		static M New();
		static M Identity();
		static M Add(M left, M right);
		static M Subtract(M left, M right);

		static M Multiply(M left, M right);

		static M Translate(Engine::Components::Vector3 position);
		static M RotateXYZ(Engine::Components::Vector3 rotation);
		static M Scale(Engine::Components::Vector3 scale);
	};
}