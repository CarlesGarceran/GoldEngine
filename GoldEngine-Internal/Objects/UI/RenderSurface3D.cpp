#include "../SDK.h"
#include "RenderSurface3D.h"

using namespace Engine::EngineObjects::Surface;
using namespace RAYMATH;
using namespace RLGL;

void onChanged(RAYLIB::RenderTexture2D& texture)
{
	RAYLIB::UnloadRenderTexture(texture);
}

void onModelUnloaded(RAYLIB::Model model)
{
	RAYLIB::UnloadModel(model);
}

void onMaterialUnloaded(RAYLIB::Material material)
{
	RAYLIB::UnloadMaterial(material);
}

Engine::EngineObjects::Surface::RenderSurface3D::RenderSurface3D(String^ name, Engine::Internal::Components::Transform^ transform)
	: RenderSurface(name, transform)
{

}

RenderSurface3D::RenderSurface3D()
	: RenderSurface()
{

}

void RenderSurface3D::Start()
{
	if (!this->tintColor)
		this->tintColor = gcnew Engine::Components::Color(0xFFFFFFFF);

	this->viewportSize = Engine::Components::Vector2(Engine::Scripting::Screen::Width, Engine::Scripting::Screen::Height);
	this->texturePtr = new Engine::Native::EnginePtr<RAYLIB::RenderTexture2D>(RAYLIB::LoadRenderTexture(viewportSize.x, viewportSize.y), onChanged, onChanged);

	RAYLIB::Mesh planeMesh = GenMeshPlane(transform->scale.x, transform->scale.y, 1, 1);
	material = new Engine::Native::EnginePtr<RAYLIB::Material>(LoadMaterialDefault(), nullptr, onMaterialUnloaded);
	model = new Engine::Native::EnginePtr<RAYLIB::Model>(LoadModelFromMesh(planeMesh), onModelUnloaded, onModelUnloaded);
	viewport = new Engine::Native::EnginePtr<RAYLIB::RenderTexture2D>(RAYLIB::LoadRenderTexture(viewportSize.x, viewportSize.y), onChanged, onChanged);
	
	this->attributes->getAttribute("viewportSize")->onPropertyChanged->connect(gcnew Action<Engine::Components::Vector2, Engine::Components::Vector2>(this, &RenderSurface3D::onViewportSizeChanged));
}

void RenderSurface3D::Update()
{
	if (failsafe)
		return;

	{
		RAYLIB::BeginTextureMode(viewport->getInstance());
		ClearBackground(RAYLIB::BLANK);

		for each(GameObject ^ child in this->GetChildren())
		{
			child->Draw();
			child->DrawGUI();
		}

		RAYLIB::EndTextureMode();
	}

	{ 
		// FLIP THE VIEWPORT BACK TO NORMAL SO IT CAN GET PROJECTED IN THE SURFACE

		RAYLIB::BeginTextureMode(this->texturePtr->getInstance());
		ClearBackground(RAYLIB::BLANK);

		RAYLIB::Rectangle inRectangle;

		inRectangle.x = 0;
		inRectangle.y = 0;
		inRectangle.width = -viewport->getInstance().texture.width;
		inRectangle.height = -viewport->getInstance().texture.height;

		RAYLIB::Rectangle outRectangle;

		outRectangle.x = 0;
		outRectangle.y = 0;
		outRectangle.width = viewport->getInstance().texture.width * transform->scale.x;
		outRectangle.height = viewport->getInstance().texture.height * transform->scale.y;

		DrawTexturePro(
			viewport->getInstance().texture,
			inRectangle,
			outRectangle,
			{ 0, 0 },
			0.0f,
			this->tintColor->toNative()
		);

		RAYLIB::EndTextureMode();
	}
}

void RenderSurface3D::Draw()
{
	if (failsafe)
		return;

	if (this->texturePtr == nullptr)
		return;

	RAYLIB::Vector3 position = this->transform->position.toNative();
	RAYLIB::Vector2 scale = this->transform->scale.toVector2().toNative();
	RAYLIB::Texture texture = this->texturePtr->getInstance().texture;

	RAYLIB::Rectangle inRectangle;

	inRectangle.x = 0;
	inRectangle.y = 0;
	inRectangle.width = texture.width;
	inRectangle.height = texture.height;

	material->getInstance().maps[0].texture = texture;
	Engine::EngineObjects::Camera^ mainCamera = Singleton<Engine::Scripting::ObjectManager^>::Instance->GetMainCamera();

	/*
	* For billboard:

	DrawBillboardRec(
			*((RAYLIB::Camera*)mainCamera->get()),
			texture,
			inRectangle,
			position,
			scale,
			tintColor->toNative()
		);
	*/

	model->getInstance().materials[0] = material->getInstance();
	model->getInstance().transform = MatrixRotateXYZ({
		transform->rotation.x * DEG2RAD,
		transform->rotation.y * DEG2RAD,
		transform->rotation.z * DEG2RAD
	});

	RAYLIB::DrawModelEx(model->getInstance(), transform->position.toNative(), {}, 0, transform->scale.toNative(), tintColor->toNative());
}

void RenderSurface3D::Destroy()
{
	failsafe = true;

	delete model;
	delete texturePtr;
	delete viewport;
	delete material;
}

void RenderSurface3D::onViewportSizeChanged(Engine::Components::Vector2 newSize, Engine::Components::Vector2 oldSize)
{
	if (newSize.Equals(oldSize)) return;

	RAYLIB::RenderTexture2D viewport = RAYLIB::LoadRenderTexture(newSize.x, newSize.y);

	this->texturePtr->setInstanceRef(viewport);
}

RenderSurface::RenderSurface(String^ name, Engine::Internal::Components::Transform^ transform) :
	Engine::EngineObjects::Script(name, transform)
{

}

RenderSurface::RenderSurface() :
	Engine::EngineObjects::Script()
{

}