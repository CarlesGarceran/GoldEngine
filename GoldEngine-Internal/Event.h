#pragma once

namespace Engine::Scripting::Events
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
	public ref class Event : Engine::Interfaces::IInstantiable<Event^>
	{
	private:
		delegate void LuaEvent(cli::array<MoonSharp::Interpreter::DynValue^>^);
		System::Collections::Generic::List<System::Object^>^ invokables;
		bool isLuaFunction = false;
		bool isDelegate = false;
		bool isAction = false;
		MoonSharp::Interpreter::Script^ scriptOwner;

	public:
		Event();

	public:
		void connect(System::Delegate^);
		void connect(System::Action^);
		void connect(MoonSharp::Interpreter::DynValue^);

		void Connect(System::Delegate^ arg) { connect(arg); };
		void Connect(System::Action^ arg) { connect(arg); };
		void Connect(MoonSharp::Interpreter::DynValue^ arg) { connect(arg); };

		void disconnect(System::Delegate^);
		void disconnect(System::Action^);
		void disconnect(MoonSharp::Interpreter::DynValue^);

		void Disconnect(System::Delegate^ arg) { disconnect(arg); };
		void Disconnect(System::Action^ arg) { disconnect(arg); };
		void Disconnect(MoonSharp::Interpreter::DynValue^ arg) { disconnect(arg); };

		void disconnectAll();
		void DisconnectAll() { disconnectAll(); };

	public:
		System::Object^ invoke();
		System::Object^ invoke(... cli::array<System::Object^>^ args);

		System::Object^ Invoke() { return invoke(); }
		System::Object^ Invoke(... cli::array<System::Object^>^ args) { return invoke(args); };

		System::Object^ raiseExecution(cli::array<System::Object^>^);
		System::Object^ raiseExecution();

	public:
		static Event^ Create();
		static Event^ New();
	};
}