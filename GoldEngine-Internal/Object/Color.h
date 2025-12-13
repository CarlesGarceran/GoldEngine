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
		unsigned char _r; 
		unsigned char _g; 
		unsigned char _b; 
		unsigned char _a;

	public:
		property unsigned char r { 
			unsigned char get(); 
			void set(unsigned char);
		}

		property unsigned char g {
			unsigned char get();
			void set(unsigned char);
		}

		property unsigned char b {
			unsigned char get();
			void set(unsigned char);
		}

		property unsigned char a {
			unsigned char get();
			void set(unsigned char);
		}

	public:
		[Newtonsoft::Json::JsonConstructorAttribute]
		Color();
		Color(unsigned int colorHex);
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		Color(int r, int g, int b, int a);
		RAYLIB::Color toNativeAlt();
		RAYLIB::Color toNative();
		RAYLIB::Vector3 toNativeVector3();
		std::array<float, 4> toFloat();
		unsigned int toHex();
		unsigned int ToHex() { return toHex(); }

		unsigned int toRGBA();
		unsigned int ToRGBA() { return toRGBA(); }

		unsigned int toARGB();
		unsigned int ToARGB() { return toARGB(); }

		void setARGB(unsigned int value);
		void SetARGB(unsigned int value) { this->setARGB(value); }

		void setRGBA(unsigned int value);
		void SetRGBA(unsigned int value) { this->setRGBA(value); }

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

		static Color^ New(int r, int g, int b, int a)
		{
			return gcnew Color(r, g, b, a);
		}

	private:
		int GetR();
		int GetG();
		int GetB();
		int GetA();

		int getR() { return GetR(); }
		int getG() { return GetG(); }
		int getB() { return GetB(); }
		int getA() { return GetA(); }

		void setR(unsigned char value);
		void setG(unsigned char value);
		void setB(unsigned char value);
		void setA(unsigned char value);

		void SetR(unsigned char value) { this->setR(value); }
		void SetG(unsigned char value) { this->setG(value); }
		void SetB(unsigned char value) { this->setB(value); }
		void SetA(unsigned char value) { this->setA(value); }

	public:
		static operator GLWrapper::Color(Engine::Components::Color^ color);
		static operator Engine::Components::Color^(GLWrapper::Color color);
	};
}