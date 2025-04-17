#pragma once

namespace Engine::Components
{
	[System::SerializableAttribute]
	[Newtonsoft::Json::JsonObjectAttribute]
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Color : public Engine::Interfaces::IInstantiable<Color^>
	{
	private:
		[Newtonsoft::Json::JsonRequiredAttribute]
		unsigned int hexColor;
		[Newtonsoft::Json::JsonRequiredAttribute]
		__int8 r;
		[Newtonsoft::Json::JsonRequiredAttribute]
		__int8 g;
		[Newtonsoft::Json::JsonRequiredAttribute]
		__int8 b;
		[Newtonsoft::Json::JsonRequiredAttribute]
		__int8 a;

	public:
		[Newtonsoft::Json::JsonConstructorAttribute]
		Color(unsigned int colorHex);
		Color(__int8 r, __int8 g, __int8 b, __int8 a);
		Color(int r, int g, int b, int a);
		RAYLIB::Color toNativeAlt();
		RAYLIB::Color toNative();
		RAYLIB::Vector3 toNativeVector3();
		float* toFloat();
		unsigned int% toHex();

		void setHex(unsigned int value);
		void SetHex(unsigned int value) { this->setHex(value); }

		void setR(__int8 value);
		void setG(__int8 value);
		void setB(__int8 value);
		void setA(__int8 value);

		void SetR(__int8 value) { this->setR(value); }
		void SetG(__int8 value) { this->setG(value); }
		void SetB(__int8 value) { this->setB(value); }
		void SetA(__int8 value) { this->setA(value); }

		static Color^ New()
		{
			return gcnew Color(0xFFFFFFFF);
		}

		static Color^ Create()
		{
			return gcnew Color(0xFFFFFFFF);
		}

		static Color^ New(unsigned int hexColor)
		{
			return gcnew Color(hexColor);
		}
	};
}