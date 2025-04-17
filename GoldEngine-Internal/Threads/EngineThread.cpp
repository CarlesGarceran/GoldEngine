#include "../SDK.h"
#include "EngineThread.h"

using namespace Engine::Threading;

NativeThread::NativeThread()
{
	Swap(nullptr);
}

NativeThread::NativeThread(void* funcPtr)
{
	Swap(funcPtr);
}

template <typename T>
NativeThread::NativeThread(std::function<T> funcPtr)
{
	Swap(funcPtr);
}

void NativeThread::Execute()
{
	if (IsBinded())
	{
		((void (*)(void))Get())(); // Convert it to a callable structure and hope it works, lol.
	}
}

void NativeThread::Free()
{
	this->Release();
}

ManagedThread::ManagedThread()
{

}

ManagedThread::ManagedThread(System::Delegate^ delegate)
{
	this->delegateFunction = delegate;
}

void ManagedThread::Execute()
{
	if (this->delegateFunction == nullptr)
		return;
	
	delegateFunction->DynamicInvoke();
}

void ManagedThread::Free()
{
	delete delegateFunction;
}