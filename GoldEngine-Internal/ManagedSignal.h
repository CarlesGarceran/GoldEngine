#pragma once

namespace Engine::Signals
{
	ref class ManagedSignal;
}

namespace Engine::Managers
{
	ref class SignalManager;
}

namespace Engine::Signals
{
	// types of signals

	public delegate void SimpleSignal();

	public delegate bool LuaSignal(System::String^);

	public delegate System::Object^ LuaSignal_V2(DynValue^);

	public delegate System::Object^ LuaSignal_V3(cli::array<DynValue^>^);

	generic<typename T>
	public delegate T SignalWithArgs(cli::array<System::Object^>^);

	generic<typename T>
	public delegate T SignalWithArg(System::Object^);

	generic<typename T>
	public delegate T Signal();

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class ManagedSignal
	{
	private:
		System::Object^ bindedSignal;
		bool closure = false;

		System::Object^ Callback()
		{
			if (bindedSignal != nullptr)
			{
				if(!closure)
					return ((System::Delegate^)bindedSignal)->DynamicInvoke();
				else
					return ((MoonSharp::Interpreter::Closure^)bindedSignal)->Call();
			}

			return nullptr;
		}

		System::Object^ Callback(cli::array<System::Object^>^ args)
		{
			if (bindedSignal != nullptr)
			{
				if (!closure)
					return ((System::Delegate^)bindedSignal)->DynamicInvoke(args);
				else
					return ((MoonSharp::Interpreter::Closure^)bindedSignal)->Call(args);
			}

			return nullptr;
		}

		Object^ invoker;

	public:
		// constructors & destructors (only called by the signalmanager)
		ManagedSignal(Engine::Managers::SignalManager^ signalManager)
		{
			if (signalManager == nullptr)
				throw gcnew System::Exception("Cannot instantiate a signal without a signal manager");
		}

		~ManagedSignal()
		{

		}

	public:
		// bind functions, to bind the incomming signals to the instance
		generic <typename T>
		void Bind(Signal<T>^ signal)
		{
			bindedSignal = signal;
		}

		void Bind(System::Delegate^ signal)
		{
			bindedSignal = signal;
		}

		generic <typename T>
		void Bind(SignalWithArg<T>^ signal)
		{
			bindedSignal = signal;
		}

		generic <typename T>
		void Bind(SignalWithArgs<T>^ signal)
		{
			bindedSignal = signal;
		}

		generic <typename T>
		void Bind(SimpleSignal^ signal)
		{
			bindedSignal = signal;
		}

		void Bind(Engine::Lua::VM::LuaVM^ vmInstance, System::String^ funcName)
		{
			DynValue^ temp = vmInstance->GetScriptState()->Globals->Get(funcName);
			invoker = funcName;

			if (temp->Type == DataType::Function)
			{
				LuaSignal^ signal = gcnew LuaSignal(vmInstance, &Engine::Lua::VM::LuaVM::InvokeFunction);
				bindedSignal = signal;
			}
		}
		
		void Bind(Engine::Lua::VM::LuaVM^ vmInstance, DynValue^ temp)
		{
			invoker = temp;

			if (temp->Type == DataType::Function)
			{
				LuaSignal_V2^ signal = gcnew LuaSignal_V2(vmInstance, &Engine::Lua::VM::LuaVM::InvokeFunctionO);
				bindedSignal = signal;
			}
		}

		void Bind(MoonSharp::Interpreter::Closure^ closure)
		{
			invoker = nullptr;
			this->closure = true;
			bindedSignal = closure;
		}

		void Bind(MoonSharp::Interpreter::DynValue^ value)
		{
			this->closure = true;
			invoker = nullptr;

			if (value->Type == DataType::Function)
			{
				bindedSignal = value->Function;
			}
		}

		void SetInvoker(System::Object^ invoker)
		{
			this->invoker = invoker;
		}

		// call fucntions (to call the binded signals)
		System::Object^ Call() 
		{
			return Callback();
		}

		System::Object^ Call(cli::array<System::Object^>^ args)
		{
			return Callback(args);
		}

		System::Object^ Invoke()
		{
			return Call();
		}

		void DisposeSignal()
		{
			bindedSignal = nullptr;
			delete this;
		}
	};

}


namespace Engine::Managers
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
	public ref class SignalManager
	{
	private:
		System::Collections::Generic::List<System::Object^>^ signals;
		System::Collections::Generic::Dictionary<String^, Engine::Signals::ManagedSignal^>^ signalMap;

	public:
		SignalManager() 
		{
			signalMap = gcnew System::Collections::Generic::Dictionary<String^, Engine::Signals::ManagedSignal^>();
			signals = gcnew System::Collections::Generic::List<System::Object^>();
			Singleton<SignalManager^>::Create(this);
		}

	public:
		bool RegisterSignal(String^ id, Engine::Signals::ManagedSignal^ signal)
		{
			if (signalMap->ContainsKey(id))
				return false;
			else
				(signalMap->Add(id, signal));

			return true;
		}

	public:
		Engine::Signals::ManagedSignal^ GetSignal(String^ id)
		{
			Engine::Signals::ManagedSignal^ outputSignal;

			signalMap->TryGetValue(id, outputSignal);

			return outputSignal;
		}

	public:
		void UnregisterSignal(String^ signalId)
		{
			signalMap->Remove(signalId);
		}

	public:
		generic <class T>
		Engine::Signals::ManagedSignal^ GetSignalByIndex(int index)
		{
			return (Engine::Signals::ManagedSignal^)signals[index];
		}

		generic <class T>
		int GetIndexBySignal(Engine::Signals::ManagedSignal^ signal)
		{
			return signals->IndexOf(signal);
		}

		bool RemoveSignal(System::Object^ signal)
		{
			return signals->Remove(signal);
		}

		Engine::Signals::ManagedSignal^ CreateSignal()
		{
			auto signal = gcnew Engine::Signals::ManagedSignal(this);
			signals->Add(signal);
			return signal;
		}

		generic <class T>
		System::Tuple<Engine::Signals::ManagedSignal^, int>^ CreateIndexedSignal()
		{
			int newIndex = signals->Count;
			auto signal = gcnew Engine::Signals::ManagedSignal(this);
			signals->Insert(newIndex, signal);

			return System::Tuple::Create<Engine::Signals::ManagedSignal^, int>(signal, newIndex);
		}
	};
}
