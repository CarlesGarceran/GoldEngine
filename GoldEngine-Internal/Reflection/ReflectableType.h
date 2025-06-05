#pragma once

namespace Engine::Reflectable::Generic
{
	generic <class T>
	public ref class Reflectable
	{
	public:
		Engine::Reflectable::ReflectableType^ type;
		String^ serializedObject;

	private:
		T Instance;

	public:
		Reflectable(T Instance);

		T operator()();
		void operator=(T value);

		T getInstance();
		void setInstance(T instance);

		void deserialize();
		void serialize();
	};
}