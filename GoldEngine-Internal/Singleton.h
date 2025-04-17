generic <class T>
public ref class Singleton
{
public:
	static T Instance;
	static bool Instantiated = false;

public:
	static void Create(T inst)
	{
		if (Singleton<T>::Instantiated)
			Singleton<T>::Release();

		Instance = inst;
		Instantiated = true;
	}

public:
	static void Release()
	{
		Instance = {};
		Instantiated = false;
	}
};

/*

template <class T>
public class NativeSingleton
{
public:
	static T* Instance;
	static bool Instantiated;

public:
	static void Create(T* inst)
	{
		if (!NativeSingleton<T>::Instantiated)
			NativeSingleton<T>::Release();

		Instance = inst;
		Instantiated = true;
	}

	static void Release()
	{
		Instance = NULL;
		Instantiated = false;
	}
};

*/

[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
private ref class LuaSingletonProxy
{
public:
	generic <typename T>
	static T Instance()
	{
		return Singleton<T>::Instance;
	}

	generic <typename T>
	static bool Instantiated()
	{
		return Singleton<T>::Instantiated;
	}

	generic <typename T>
	static void Create(T instance)
	{
		return Singleton<T>::Create(instance);
	}
};

[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
[Engine::Attributes::LuaAPIAttribute]
public ref class SharedInstance
{
public:
	System::Object^ Instance;

public:
	SharedInstance(System::Object^ inst)
	{
		Instance = inst;
	}

private:
	static System::Collections::Generic::Dictionary<System::String^, SharedInstance^>^ Instances = gcnew System::Collections::Generic::Dictionary<System::String^, SharedInstance^>();

public:
	static void Create(System::String^ name, System::Object^ i)
	{
		if (!Instances->ContainsKey(name))
		{
			Instances->Add(name, gcnew SharedInstance(i));
		}
		else
		{
			Instances[name]->Instance = i;
		}
	}

	generic <class T>
	static void Create(System::String^ name, T i)
	{
		if (!Instances->ContainsKey(name))
		{
			Instances->Add(name, gcnew SharedInstance(i));
		}
		else
		{
			Instances[name]->Instance = i;
		}
	}

public:
	static bool ExistsInstance(System::String^ name)
	{
		return Instances->ContainsKey(name);
	}

public:
	static System::Object^ Get(System::String^ name)
	{
		if (!ExistsInstance(name))
			return nullptr;

		return Instances[name]->Instance;
	}

	generic <typename T>
	static T Get(System::String^ name)
	{
		return safe_cast<T>(Instances[name]->Instance);
	}

public:
	static void Release(System::String^ name)
	{
		Instances[name]->Instance = nullptr;
		Instances->Remove(name);
	}
};