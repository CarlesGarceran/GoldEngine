#pragma once

namespace Engine::Reflectable::Generic
{
	generic <class T>
	public ref class Reflectable
	{
	public:
		T Instance;
		Engine::Reflectable::ReflectableType^ type;


	public:
		Reflectable(T Instance);

		[Newtonsoft::Json::JsonConstructorAttribute]
		Reflectable()
		{
			deserialize();
		}

		T operator->();
		T operator=(T value);
		void operator[](T value);

	private:
		void deserialize();
	};
}