#pragma once

namespace Engine::Components::Locs::Generic
{
	public ref class MaterialLoc
	{
	public:
		MaterialLoc() {}
		virtual unsigned int GetLocType() { return 0; }
	};

	public ref class ShaderLoc
	{
	public:
		System::String^ locName;
		Engine::Reflectable::ReflectableType^ type;
		System::String^ locData;

	private:
		MaterialLoc^ reference;

	public:
		ShaderLoc() 
		{
			if (locName == nullptr || type == nullptr || locData == nullptr)
				return;

			deserialize();
		}

		ShaderLoc(String^ locName, MaterialLoc^ locRef)
		{
			this->locName = locName;
			this->reference = locRef;

			type = gcnew Engine::Reflectable::ReflectableType(locRef->GetType());
			locData = Serialize(reference);
		}

		void deserialize()
		{
			this->reference = (MaterialLoc^)Deserialize(this->locData, this->type->getTypeReference());
		}

		void serialize()
		{
			type = gcnew Engine::Reflectable::ReflectableType(this->reference->GetType());
			locData = Serialize(this->reference);
		}

	public:
		void SetMaterialLocation(MaterialLoc^ locRef)
		{
			this->reference = locRef;

			type = gcnew Engine::Reflectable::ReflectableType(locRef->GetType());
			locData = Serialize(reference);
		}

		MaterialLoc^ GetMaterialLocation()
		{
			return this->reference;
		}
	};
}

namespace Engine::Components
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		public ref class Material
	{
	public:
		Engine::Reflectable::Generic::Reflectable<unsigned int>^ shaderId;
		System::Collections::Generic::List<Locs::Generic::ShaderLoc^>^ MaterialProperties;

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

		void SerializeProperties();
		void DeserializeProperties();

		static Material^ Create();
		static Material^ New();
	};
}