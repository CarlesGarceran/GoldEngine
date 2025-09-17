#pragma once

#include "functional"

namespace Engine::Native
{
	template <typename T>
	class EnginePtr
	{
	private:
		T ptrInstance;
		std::function<void(T)> onObjectDeleted;
		std::function<void(T)> onObjectChanged;
		bool loaded = true;
		
	public:
		EnginePtr()
		{
			ptrInstance = T();
			onObjectDeleted = NULL;
			onObjectChanged = NULL;

			loaded = false;
		}

		EnginePtr(T instance, std::function<void(T)> onDeleted = NULL, std::function<void(T)> onChanged = NULL)
		{
			this->ptrInstance = instance;
			this->onObjectDeleted = onDeleted;
			this->onObjectChanged = onChanged;

			loaded = true;
		}

		~EnginePtr()
		{
			this->loaded = false;

			if (onObjectDeleted != nullptr)
				onObjectDeleted(ptrInstance);
		}

		bool isLoaded() { return this->loaded; }

		T& getInstance()
		{
			return ptrInstance;
		}

		void setInstance(T instance)
		{
			if (onObjectChanged != nullptr)
				onObjectChanged(ptrInstance);

			if ((&instance) != NULL)
				this->loaded = true;
			else
				this->loaded = false;

			this->ptrInstance = instance;
		}

		void setInstanceRef(T& instance)
		{
			if(onObjectChanged != nullptr)
				onObjectChanged(ptrInstance);

			this->loaded = true;
			this->ptrInstance = instance;
		}

		void free()
		{
			this->loaded = false;

			if (onObjectDeleted != nullptr)
				onObjectDeleted(ptrInstance);

			this->ptrInstance = T();
		}

		T& release()
		{
			this->loaded = false;
			T inst = this->ptrInstance;
			this->ptrInstance = nullptr;
			return inst;
		}

		void destroy() 
		{
			delete this;
		}
	};
}