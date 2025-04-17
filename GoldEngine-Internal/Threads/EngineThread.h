#pragma once

namespace Engine::Threading
{
	template <typename T>
	private struct Swappable
	{
	private:
		T funcPtr = nullptr;
		bool bindedFunction = false;

	public: 
		void Swap(T newPtr)
		{
			if (bindedFunction == true)
				delete funcPtr;

			funcPtr = newPtr;

			if(funcPtr != nullptr)
				bindedFunction = true;
		}

		void Release()
		{
			bindedFunction = false;
			delete funcPtr;
		}

		T Get()
		{
			return funcPtr;
		}

		bool IsBinded()
		{
			return bindedFunction;
		}
	};

	public class NativeThread : private Swappable<void*>
	{
	public:
		NativeThread();
		NativeThread(void* funcPtr);
		template <typename T>
		NativeThread(std::function<T> func);

		void Execute();
		void Free();
	};

	public ref class ManagedThread
	{
	private:
		System::Delegate^ delegateFunction;

	public:
		ManagedThread();
		ManagedThread(System::Delegate^ delegate);

		void Execute();
		void Free();
	};
}