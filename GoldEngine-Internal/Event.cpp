#include "Includes.h"
#include "ManagedIncludes.h"
#include "GlIncludes.h"
#include "CastToNative.h"
#include "LoggingAPI.h"
#include "Event.h"

using namespace Engine::Scripting::Events;

Event::Event()
{
	this->invokables = gcnew System::Collections::Generic::List<System::Object^>();
	this->isLuaFunction = false;
	this->isAction = false;
	this->isDelegate = false;
}

void Event::connect(System::Delegate^ delegate)
{
	this->invokables->Add(delegate);
	this->isLuaFunction = false;
	this->isAction = false;
	this->isDelegate = true;
}

void Event::connect(System::Action^ delegate)
{
	this->invokables->Add(delegate);
	this->isLuaFunction = false;
	this->isDelegate = false;
	this->isAction = true;
}

void Event::connect(MoonSharp::Interpreter::DynValue^ function)
{
	if (function->Type == MoonSharp::Interpreter::DataType::Function)
	{
		this->invokables->Add(function->Function->GetDelegate());
		this->isLuaFunction = true;
		this->isAction = false;
		this->isDelegate = false;
	}
}

void Event::disconnect(System::Delegate^ delegate)
{
	this->invokables->Remove(delegate);
	this->isLuaFunction = false;
	this->isAction = false;
	this->isDelegate = false;
}

void Event::disconnect(System::Action^ delegate)
{
	this->invokables->Remove(delegate);
	this->isLuaFunction = false;
	this->isAction = false;
	this->isDelegate = false;
}

void Event::disconnect(MoonSharp::Interpreter::DynValue^ delegate)
{
	this->invokables->Remove(delegate);
	this->isLuaFunction = false;
	this->isAction = false;
	this->isDelegate = false;
}

System::Object^ Event::invoke()
{
	try
	{
		if (invokables == nullptr)
			return nullptr;

		for each (System::Object ^ pInvokable in invokables)
		{
			if (isLuaFunction)
			{
				MoonSharp::Interpreter::ScriptFunctionDelegate^ delegate = (MoonSharp::Interpreter::ScriptFunctionDelegate^)pInvokable;

				return delegate->Invoke();
			}
			else if (isDelegate)
			{
				System::Delegate^ delegate = (System::Delegate^)pInvokable;

				return delegate->DynamicInvoke();
			}
			else if (isAction)
			{
				System::Action^ delegate = (System::Action^)pInvokable;

				return delegate->DynamicInvoke();
			}
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ exception)
	{
		printError(exception->Message);
		printError(exception->StackTrace);
		printError("Lua Error Inspector:");
		printError(exception->DecoratedMessage);
	}
	catch (System::Exception^ exception)
	{
		printError(exception->Message);
		printError(exception->StackTrace);
	}

	return nullptr;
}

System::Object^ Event::invoke(cli::array<System::Object^>^ objects)
{
	try
	{
		if (this == nullptr)
			return nullptr;

		if (invokables == nullptr)
			return nullptr;

		for each (System::Object ^ pInvokable in invokables)
		{
			if (isLuaFunction)
			{
				MoonSharp::Interpreter::ScriptFunctionDelegate^ delegate = (MoonSharp::Interpreter::ScriptFunctionDelegate^)pInvokable;

				return delegate->Invoke(objects);
			}
			else if (isDelegate)
			{
				System::Delegate^ delegate = (System::Delegate^)pInvokable;

				return delegate->DynamicInvoke(objects);
			}
			else if (isAction)
			{
				System::Action^ delegate = (System::Action^)pInvokable;

				return delegate->DynamicInvoke(objects);
			}
		}
	}
	catch (MoonSharp::Interpreter::ScriptRuntimeException^ exception)
	{
		printError(exception->Message);
		printError(exception->StackTrace);
		printError("Lua Error Inspector:");
		printError(exception->DecoratedMessage);
	}
	catch (System::Exception^ exception)
	{
		printError(exception->Message);
		printError(exception->StackTrace);
	}

	return nullptr;
}

System::Object^ Event::raiseExecution(cli::array<System::Object^>^ objects)
{
	return invoke(objects);
}


System::Object^ Event::raiseExecution()
{
	return invoke();
}

void Event::disconnectAll()
{
	this->invokables->Clear();
}

Event^ Event::Create()
{
	return gcnew Event();
}

Event^ Event::New()
{
	return gcnew Event();
}