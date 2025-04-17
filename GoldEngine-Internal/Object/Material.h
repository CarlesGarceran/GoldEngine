#pragma once

namespace Engine::Components::Locs::Generic
{
	public ref class MaterialLoc abstract
	{
	public:
		virtual unsigned int GetLocType() { return 0; };
	};
}

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		public ref class Material
	{
	public:
		unsigned int shaderId;
		System::Collections::Generic::Dictionary<System::String^, Locs::Generic::MaterialLoc^>^ MaterialProperties;

	public:
		Material(unsigned int shaderId);

		[Newtonsoft::Json::JsonConstructorAttribute]
		Material();

	public:
		bool AddProperty(System::String^ propertyName, Locs::Generic::MaterialLoc^ value);
		bool UpdateProperty(System::String^ propertyName, Locs::Generic::MaterialLoc^ value);
		bool RemoveProperty(System::String^ name);

		generic <class T>
		T GetMaterialProperty(System::String^ name);

		Locs::Generic::MaterialLoc^ GetMaterialProperty(System::String^ name);
		Locs::Generic::MaterialLoc^ GetBaseColor();
		unsigned int GetMainTexture();
		unsigned int GetNormalMap();

		void SetMainTexture(int textureId);
		void SetNormalMap(int textureId);

		static Material^ Create();
		static Material^ New();
	};
}