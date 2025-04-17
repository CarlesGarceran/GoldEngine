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

		void disconnect(System::Delegate^);
		void disconnect(System::Action^);
		void disconnect(MoonSharp::Interpreter::DynValue^);

		void disconnectAll();

	public:
		System::Object^ invoke(cli::array<System::Object^>^);
		System::Object^ invoke();
		System::Object^ raiseExecution(cli::array<System::Object^>^);
		System::Object^ raiseExecution();

	public:
		static Event^ Create();
		static Event^ New();
	};
}