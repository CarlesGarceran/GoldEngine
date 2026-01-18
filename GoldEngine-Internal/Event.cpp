#include "Includes.h"
#include "ManagedIncludes.h"
#include "GlIncludes.h"
#include "CastToNative.h"
#include "LoggingAPI.h"
#include "Event.h"

using namespace Engine::Scripting::Events;

Event::Event()
{
	this->invokables = gcnew System::Collections::Generic::List<Invokable^>();
}

Event::~Event()
{
	this->!Event();
}

Event::!Event()
{

}

void Event::connect(System::Delegate^ delegate)
{
	this->invokables->Add(gcnew Invokable(delegate));
}

void Event::connect(MoonSharp::Interpreter::DynValue^ function)
{
	if (function->Type == MoonSharp::Interpreter::DataType::Function)
	{
		this->invokables->Add(gcnew Invokable(function->Function->GetDelegate()));
	}
}

void Event::disconnect(System::Delegate^ del)
{
	for (int i = invokables->Count - 1; i >= 0; i--)
	{
		if (invokables[i]->type == Invokable::Type::Delegate &&
			invokables[i]->target == del)
		{
			invokables->RemoveAt(i);
		}
	}
}

void Event::disconnect(MoonSharp::Interpreter::DynValue^ del)
{
	if (del->Type == MoonSharp::Interpreter::DataType::Function && del->Function != nullptr)
	{
		for (int i = invokables->Count - 1; i >= 0; i--)
		{
			if (invokables[i]->type == Invokable::Type::Lua &&
				invokables[i]->target == del->Function->GetDelegate())
			{
				invokables->RemoveAt(i);
			}
		}
	}
}

cli::array<System::Object^>^ Event::invoke()
{
	try
	{
		if (invokables == nullptr)
			return nullptr;

		List<System::Object^>^ results = gcnew List<System::Object^>();

		for each (Invokable ^ inv in invokables)
		{
			switch (inv->type)
			{
			case Invokable::Type::Lua:
				results->Add(((MoonSharp::Interpreter::ScriptFunctionDelegate^)inv->target)->Invoke());
				break;
			case Invokable::Type::Delegate:
				results->Add(((Delegate^)inv->target)->DynamicInvoke());
				break;
			}
		}

		return results->ToArray();
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

cli::array<System::Object^>^ Event::invoke(... cli::array<System::Object^>^ objects)
{
	try
	{
		if (this == nullptr)
			return nullptr;

		if (invokables == nullptr)
			return nullptr;

		List<System::Object^>^ results = gcnew List<System::Object^>();

		for each (Invokable ^ inv in invokables)
		{
			switch (inv->type)
			{
			case Invokable::Type::Lua:
				results->Add(((MoonSharp::Interpreter::ScriptFunctionDelegate^)inv->target)->Invoke(objects));
				break;
			case Invokable::Type::Delegate:
				results->Add(((Delegate^)inv->target)->DynamicInvoke(objects));
				break;
			}
		}

		return results->ToArray();
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

cli::array<System::Object^>^ Event::raiseExecution(cli::array<System::Object^>^ objects)
{
	try
	{
		if (this == nullptr)
			return nullptr;

		if (invokables == nullptr)
			return nullptr;

		List<System::Object^>^ results = gcnew List<System::Object^>();

		for each (Invokable ^ inv in invokables)
		{
			switch (inv->type)
			{
			case Invokable::Type::Lua:
				results->Add(((MoonSharp::Interpreter::ScriptFunctionDelegate^)inv->target)->Invoke(objects));
				break;
			case Invokable::Type::Delegate:
				results->Add(((Delegate^)inv->target)->DynamicInvoke(objects));
				break;
			}
		}

		return results->ToArray();
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


cli::array<System::Object^>^ Event::raiseExecution()
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