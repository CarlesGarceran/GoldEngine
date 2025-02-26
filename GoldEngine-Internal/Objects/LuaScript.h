#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		public ref class LuaScript : public Engine::EngineObjects::Script
	{
	private:
		String^ luaSource = "";
	public:
		[Newtonsoft::Json::JsonIgnoreAttribute]
			Engine::Lua::VM::LuaVM^ virtualMachine;

		String^ luaFilePath = "./";

	private:
		bool loadErrorCalledBack = false;

	public:
		LuaScript(System::String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
		{

		}

		void Destroy() override
		{
			if (virtualMachine != nullptr)
			{
				delete virtualMachine;
				virtualMachine = nullptr;
			}
		}

	public:
		void Reset()
		{
			if (virtualMachine != nullptr)
			{
				delete virtualMachine;
				virtualMachine = nullptr;
			}

			initVM();
			Start();
		}

	private:
		void initVM()
		{
			if (File::Exists(luaFilePath))
			{
				printWarning("Loading Lua binary: " + luaFilePath);

				MoonSharp::Interpreter::Script::DefaultOptions->CheckThreadAccess = false;
				virtualMachine = gcnew Engine::Lua::VM::LuaVM();
				virtualMachine->GetScriptState()->Options->CheckThreadAccess = false;

				luaSource = virtualMachine->ReadFromFile(luaFilePath);

				virtualMachine->RegisterGlobal("script", this);
				virtualMachine->RegisterGlobal("attributes", attributes);
				virtualMachine->RegisterGlobal("Globals", virtualMachine->GetGlobals());

				virtualMachine->RegisterScript(luaSource);

				//Start();
			}
			else
			{
				if (!loadErrorCalledBack)
				{
					print("[IO/EX]:", "Lua binary: " + luaFilePath + " file does not exist");
					loadErrorCalledBack = true;
				}
			}
		}

	public:
		void LoadSource(String^ source)
		{
			luaSource = source;
		}

		void LoadFromFile(String^ file)
		{
			luaSource = File::ReadAllText(file);
		}

	public:
		void Init(Object^ object) override
		{
			try
			{
				LuaScript^ luaSrc = Cast::Dynamic<LuaScript^>(object);

				if (luaSrc != nullptr)
				{
					if (luaSrc->luaSource != nullptr)
						this->luaSource = luaSrc->luaSource;
					else
						printError("lua source is nullptr, wtf is going on\n");
				}
				else
				{
					printError("luasrc is not a valid instance\n");
				}

				if (virtualMachine != nullptr)
				{
					delete virtualMachine;
				}

				initVM();
			}
			catch (Exception^ ex)
			{
				printError("Failed converting object to gc_luascript\n");
			}
		}

		void Start() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
			{
				printError("Scene has not finished loading, aborting execution");
				return;
			}

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("Start");
		}

		void Draw() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
				return;

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("Draw");
		}

		void Update() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
				return;

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("Update");
		}

		void PhysicsUpdate() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
				return;

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("PhysicsUpdate");
		}

		void DrawGizmo() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
				return;

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("DrawGizmo");
		}

		void DrawImGUI() override
		{
			if (!Engine::Management::Scene::getLoadedScene()->sceneLoaded())
				return;

			if (virtualMachine == nullptr)
				initVM();

			if (virtualMachine != nullptr)
				virtualMachine->InvokeFunction("DrawImGUI");
		}
	};
}