#include "Macros.h"
#include "SDK.h"
#include "Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "GameWindow.h"

#include "LuaVM.h"

#include "EngineGC.h"
#include "DataManager.h"
#include "Objects/Private/Scene.h"
#include "LogFileReporter.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneFormat.h"
#include "ManagedSignal.h"

#include "RenderPipelines/LitPBR_SRP.h"

#ifdef USE_BULLET_PHYS

#include "Objects/Physics/CollisionType.h"
#include "Objects/Physics/Native/NativePhysicsService.h"
#include "Objects/Physics/RigidBody.h"
#include "Objects/Physics/Triggers/Collider.h"
#include "Objects/Physics/PhysicsService.h"

#include "Objects/Physics/Colliders/BoxCollider.h"

#endif

using namespace Engine;
using namespace Engine::EngineObjects;
using namespace Engine::EngineObjects::Native;
using namespace Engine::Internal;
using namespace Engine::Internal::Components;
using namespace Engine::Management;
using namespace Engine::Management::MiddleLevel;
using namespace Engine::Managers;
using namespace Engine::Scripting;

extern DataPacks dataPack;
extern unsigned int passwd;
extern int max_lights;

static System::Reflection::Assembly^ ResolveAssembly(System::Object^ sender, System::ResolveEventArgs^ args)
{
	System::Reflection::AssemblyName^ requestedName = gcnew System::Reflection::AssemblyName(args->Name);

	for each (System::Reflection::Assembly ^ loadedAssembly in System::AppDomain::CurrentDomain->GetAssemblies())
	{
		if (System::Reflection::AssemblyName::ReferenceMatchesDefinition(gcnew System::Reflection::AssemblyName(loadedAssembly->FullName), requestedName))
			return loadedAssembly;
	}

	System::String^ assemblyFolder = System::IO::Path::Combine(AppDomain::CurrentDomain->BaseDirectory, "Bin/");
	System::String^ assemblyPath = System::IO::Path::Combine(assemblyFolder, requestedName->Name + ".dll");

	if (File::Exists(assemblyPath))
	{
		return System::Reflection::Assembly::LoadFrom(assemblyPath);
	}

	return nullptr;
}



static Newtonsoft::Json::JsonSerializerSettings^ SerializerSettings()
{
	auto serializerSettings = gcnew Newtonsoft::Json::JsonSerializerSettings();
	serializerSettings->Converters->Add(gcnew Newtonsoft::Json::Converters::StringEnumConverter());
	serializerSettings->Converters->Add(gcnew Newtonsoft::Json::Converters::KeyValuePairConverter());
	serializerSettings->TypeNameHandling = Newtonsoft::Json::TypeNameHandling::None;
	serializerSettings->PreserveReferencesHandling = PreserveReferencesHandling::Objects;

	return serializerSettings;
}

static void engine_keybinds()
{
	if (InputManager::IsKeyPressed(KeyCodes::KEY_F11))
		ToggleFullscreen();
}

static void SetupAssemblyResolver()
{
	AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&ResolveAssembly);
}

static void engine_bootstrap()
{
	SetupAssemblyResolver();
	Newtonsoft::Json::JsonConvert::DefaultSettings = gcnew Func<Newtonsoft::Json::JsonSerializerSettings^>(&SerializerSettings);

	if (!Directory::Exists("Bin/"))
		Directory::CreateDirectory("Bin");

	if (!Directory::Exists("Data"))
		Directory::CreateDirectory("Data");

	if (!Directory::Exists("Cfg"))
		Directory::CreateDirectory("Cfg");

	if (!Directory::Exists("Data/Keys/"))
		Directory::CreateDirectory("Data/Keys/");

	if (!File::Exists("Data/Keys/map.iv"))
	{
		String^ guid = System::Guid::NewGuid().ToString();
		File::WriteAllText("Data/Keys/map.iv", guid);
	}

	if (!Directory::Exists("Data/UserData/"))
		Directory::CreateDirectory("Data/UserData/");
}


extern std::string fileName;

GameWindow::GameWindow()
{
#if HIDE_CONSOLE == true
	WinAPI::FreeCons();
#endif

	printConsole(RUNTIME_VERSION);

	engine_bootstrap();

	assemblies = gcnew System::Collections::Generic::List<EngineAssembly^>();
	dataPack = DataPacks();

	assemblies->Add(gcnew EngineAssembly(System::Reflection::Assembly::GetExecutingAssembly()));

	for each (String ^ fileName in Directory::GetFiles("Bin\\"))
	{
		if (fileName->Contains(".goldasm") || fileName->Contains(".dll"))
		{
			String^ path = Directory::GetCurrentDirectory() + "\\" + fileName;

			assemblies->Add(gcnew EngineAssembly(path));
		}
	}

	SceneManager::SetAssemblyManager(assemblies);

	Start();
}

void GameWindow::Start()
{
	auto secrets = Engine::Config::EngineSecrets::ImportSecrets("./Data/Keys/secrets.dat");

	unsigned int password = Engine::Encryption::CypherLib::GetPasswordBytes(secrets->encryptionPassword);
	passwd = password;

	auto config = Engine::Config::EngineConfiguration::ImportConfig("./Data/appinfo.dat", password);

	config->windowFlags = config->_windowFlags->toWindowFlags();

	int x, y, w, h;

	x = config->resolution->x;
	y = config->resolution->y;
	w = config->resolution->w;
	h = config->resolution->h;

	SetWindowFlags(config->windowFlags);

	OpenWindow(800, 600, config->getWindowName().c_str());

	if (w == -1)
	{
		int monitor = GetCurrentMonitor();
		int monitorWidth = GetMonitorWidth(monitor);
		w = monitorWidth;
	}

	if (h == -1)
	{
		int monitor = GetCurrentMonitor();
		int monitorHeight = GetMonitorHeight(monitor);
		h = monitorHeight;
	}

	if (x == -1) x = (GetMonitorWidth(GetCurrentMonitor()) - w) / 2;
	if (y == -1) y = ((GetMonitorHeight(GetCurrentMonitor()) - h) / 2) + 24;

	SetWindowSize(w, h);
	SetWindowPosition(x, y);
	SetFPS(config->targetFPS);

	gcnew Engine::Managers::SignalManager();

	if (!Directory::Exists(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/'))))
	{
		Directory::CreateDirectory(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath->Substring(0, config->logPath->IndexOf('/')));
	}

	gcnew Engine::Utils::LogReporter(System::Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData) + "/../LocalLow/" + config->logPath);

	LayerManager::RegisterDefaultLayers();

	Preload();

	Loop();
}

void GameWindow::Draw()
{
	renderPipeline->ExecuteRenderWorkflow(this, scene);
}

void GameWindow::DrawImGui()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	auto viewPort = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(ImGui::GetID("MAIN", "vport"), viewPort, ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
}

void GameWindow::Exit()
{
	Engine::Internal::DataManager::HL_FreeAll();
	Engine::Utils::LogReporter::singleton->CloseThread();

	CloseWindow();
	exit(0);
}

void GameWindow::Update()
{
	if (Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance != renderPipeline)
		renderPipeline = Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance;

	for each (GameObject ^ obj in scene->GetRenderQueue())
	{
		if (obj != nullptr)
		{
			obj->GameUpdate();
		}
	}

	engine_keybinds();

	Engine::GC::EngineGC::Update();
}

void GameWindow::Preload()
{
	dataPack.LoadDefaultAssets();
	renderPipeline = gcnew Engine::Render::Pipelines::LitPBR_SRP();

	SceneManager::LoadSceneFromFile(gcnew System::String(fileName.c_str()), passwd, scene);

	while (!scene->sceneLoaded())
	{
		RAYLIB::WaitTime(1.0f);
	}

	packedData = scene->getSceneDataPack();

	renderPipeline = Singleton<Engine::Render::ScriptableRenderPipeline^>::Instance;

	Init();
}

void GameWindow::Init()
{
	while (!scene->sceneLoaded())
		WaitTime(1.0);

	create();

	Logging::LogCustom("[GL Version]:", "Current OpenGL version is -> " + RLGL::rlGetVersion() + ".");

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	ImGui::GetIO().ConfigErrorRecovery = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableAssert = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableDebugLog = false;
	ImGui::GetIO().ConfigErrorRecoveryEnableTooltip = false;
}


void GameWindow::create()
{
#ifdef USE_ILLUMINA
	LightManager^ lightManager = nullptr;
#endif

	Engine::EngineObjects::Private::Scene^ gameRoot = nullptr;

	if (!scene->ExistsMember("game"))
	{
		gameRoot = gcnew Engine::EngineObjects::Private::Scene(
			"game",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			)
		);
		scene->PushToRenderQueue(gameRoot);
	}
	else
	{
		try
		{
			gameRoot = scene->GetMember("game")->ToObjectType<Engine::EngineObjects::Private::Scene^>();
		}
		catch (Exception^ ex)
		{
			gameRoot = gcnew Engine::EngineObjects::Private::Scene(
				"game",
				gcnew Engine::Internal::Components::Transform(
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					nullptr
				)
			);
		}
	}

	auto workspace = scene->GetDatamodelMember("workspace", true);
	workspace->setParent(gameRoot);
	auto editor_only = scene->GetDatamodelMember("editor only", true);
	editor_only->setParent(gameRoot);
	auto gui = scene->GetDatamodelMember("gui", true);
	gui->setParent(gameRoot);
	auto daemonParent = scene->GetDatamodelMember("daemons", true);
	daemonParent->setParent(gameRoot);

#ifdef USE_BULLET_PHYS

	if (!scene->ExistsMember("PhysicsService"))
	{
		auto physicsService = gcnew Engine::EngineObjects::Physics::PhysicsService("PhysicsService",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			)
		);

		scene->PushToRenderQueue(physicsService);
	}

#endif

#ifdef USE_ILLUMINA
	if (!scene->ExistsMember("lighting"))
	{
		lightManager = gcnew LightManager("lighting",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			"Data/Engine/Shaders/Illumina/Illumina.vert",
			"Data/Engine/Shaders/Illumina/Illumina.frag"
		);
		lightManager->setParent(gameRoot);
		lightManager->protectMember();

		scene->PushToRenderQueue(lightManager);
	}
	else
	{
		try
		{
			lightManager = scene->GetMember("lighting")->ToObjectType<LightManager^>();
		}
		catch (Exception^ ex)
		{
			lightManager = lightManager = gcnew LightManager("lighting",
				gcnew Engine::Internal::Components::Transform(
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					Engine::Components::Vector3(0, 0, 0),
					nullptr
				),
				"Data/Engine/Shaders/Illumina/Illumina.vert",
				"Data/Engine/Shaders/Illumina/Illumina.frag"
			);
		}
		lightManager->setParent(gameRoot);
		lightManager->protectMember();
	}

	if (ObjectManager::singleton()->GetChildrenOf(daemonParent)->Count <= 0)
	{
		auto lightdm = gcnew Engine::EngineObjects::Daemons::LightDaemon("lightdm",
			gcnew Engine::Internal::Components::Transform(
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				Engine::Components::Vector3(0, 0, 0),
				nullptr
			),
			lightManager
		);
		lightdm->SetParent(daemonParent);
		scene->PushToRenderQueue(lightdm);
	}
#endif

}