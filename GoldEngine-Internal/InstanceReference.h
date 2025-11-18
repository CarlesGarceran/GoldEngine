#pragma once

namespace Engine::Scripting
{
	generic <class T> where T : Engine::Internal::Components::GameObject
	public value struct InstanceReference
	{
	private:
		[Newtonsoft::Json::JsonPropertyAttribute] System::String^ InstanceID;

	public:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property T Instance 
		{
		public:
			T get();
			void set(T value);
		}

	public:
		InstanceReference(T instance);
		bool HasInstance();

		T operator->();

		static operator InstanceReference<T>(T value);
		static operator T(InstanceReference<T> value);
	};
}