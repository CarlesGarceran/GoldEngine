#pragma once

namespace Engine::Scripting::Events
{
	ref struct Invokable
	{
		System::Object^ target;
		enum class Type { Lua, Delegate } type;

	public:
		inline Invokable(System::Delegate^ delegate)
			: target(delegate), type(Type::Delegate) {}

		inline Invokable(MoonSharp::Interpreter::ScriptFunctionDelegate^ delegate)
			: target(delegate), type(Type::Lua) {
		}
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
	public ref class Event : Engine::Interfaces::IInstantiable<Event^>
	{
	private:
		delegate void LuaEvent(cli::array<MoonSharp::Interpreter::DynValue^>^);
		System::Collections::Generic::List<Invokable^>^ invokables;

	public:
		Event();
		~Event();
		!Event();

	public:
		void connect(System::Delegate^);
		void connect(MoonSharp::Interpreter::DynValue^);

		void Connect(System::Delegate^ arg) { connect(arg); };
		void Connect(MoonSharp::Interpreter::DynValue^ arg) { connect(arg); };

		void disconnect(System::Delegate^);
		void disconnect(MoonSharp::Interpreter::DynValue^);

		void Disconnect(System::Delegate^ arg) { disconnect(arg); };
		void Disconnect(MoonSharp::Interpreter::DynValue^ arg) { disconnect(arg); };

		void disconnectAll();
		void DisconnectAll() { disconnectAll(); };

	public:
		cli::array<System::Object^>^ invoke();
		cli::array<System::Object^>^ invoke(... cli::array<System::Object^>^ args);

		cli::array<System::Object^>^ Invoke() { return invoke(); }
		cli::array<System::Object^>^ Invoke(... cli::array<System::Object^>^ args) { return invoke(args); };

		cli::array<System::Object^>^ raiseExecution(cli::array<System::Object^>^);
		cli::array<System::Object^>^ raiseExecution();

	public:
		static Event^ Create();
		static Event^ New();
	};
}