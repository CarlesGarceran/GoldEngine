#include "Includes.h"
#include "CastToNative.h"
#include "GlIncludes.h"
#include "../SDK.h"
#include "LoggingAPI.h"
#include "Object/Vector2.h"
#include "Object/Color.h"
#include "Object/Vector3.h"
#include "GraphicsWrapper.h"
#include "DataPacks.h"

using namespace Engine::Internal;

RenderTexture& GraphicsWrapper::GL_GetDepthTexture(int width, int height)
{
	RenderTexture2D target = LoadRenderTexture(width, height);;

	target.id = RLGL::rlLoadFramebuffer();

	if (target.id > 0)
	{
		RLGL::rlEnableFramebuffer(target.id);


		target.texture.id = RLGL::rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
		target.texture.width = width;
		target.texture.height = height;
		target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		target.texture.mipmaps = 1;

		target.depth.id = RLGL::rlLoadTextureDepth(width, height, false);
		target.depth.width = width;
		target.depth.height = height;
		target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
		target.depth.mipmaps = 1;

		RLGL::rlFramebufferAttach(target.id, target.texture.id, RLGL::RL_ATTACHMENT_COLOR_CHANNEL0, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);
		RLGL::rlFramebufferAttach(target.id, target.depth.id, RLGL::RL_ATTACHMENT_DEPTH, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);

		if (RLGL::rlFramebufferComplete(target.id))
			printWarning(gcnew String(TextFormat("FBO: [ID %i] Framebuffer object created successfully", target.id)));

		RLGL::rlDisableFramebuffer();
	}
	else 
		printWarning(gcnew String("FBO: Framebuffer object can not be created"));

	return target;
}

void GraphicsWrapper::GL_SetShaderValue(RAYLIB::Shader& shader, int loc, void* value, rlWrapper::UniformType flags)
{
	RAYLIB::SetShaderValue(shader, loc, value, (int)flags);
}

void GraphicsWrapper::GL_SetShaderValue(RAYLIB::Shader& shader, rlWrapper::ShaderLocs loc, void* value, rlWrapper::UniformType flags)
{
	RAYLIB::SetShaderValue(shader, (int)loc, value, (int)flags);
}

int GraphicsWrapper::GL_GetShaderLoc(RAYLIB::Shader& shader, const char* loc_name)
{
	return RAYLIB::GetShaderLocation(shader, loc_name);
}

RAYLIB::Model& GraphicsWrapper::GL_LoadModel(String^ fileName)
{
	return RAYLIB::LoadModel(CastStringToNative(fileName).c_str());
}

void GraphicsWrapper::GL_BeginTextureMode(RAYLIB::RenderTexture render_texture)
{
	RAYLIB::BeginTextureMode(render_texture);
}

RAYLIB::RenderTexture& GraphicsWrapper::GL_LoadRenderTexture(int width, int height)
{
	return RAYLIB::LoadRenderTexture(width, height);
}

void GraphicsWrapper::GL_BeginShaderMode(RAYLIB::Shader shader)
{
	RAYLIB::BeginShaderMode(shader);
}

void GraphicsWrapper::GL_EndShaderMode()
{
	RAYLIB::EndShaderMode();
}

void GraphicsWrapper::GL_EndTextureMode()
{
	RAYLIB::EndTextureMode();
}

void GraphicsWrapper::GL_LoadDepthTexture(RAYLIB::RenderTexture& renderTexture, int width, int height, bool renderBuffer)
{
	renderTexture.depth.width = width;
	renderTexture.depth.height = height;
	renderTexture.depth.id = RLGL::rlLoadTextureDepth(width, height, renderBuffer);
}

void GraphicsWrapper::GL_DrawModel(RAYLIB::Model& model, Engine::Components::Vector3^ position, float scale, Engine::Components::Color^ color)
{
	RAYLIB::DrawModel(model, position->toNative(), scale, color->toNative());
}

void GraphicsWrapper::GL_DrawTexture(RAYLIB::Texture& texture, Engine::Components::Vector2^ position, Engine::Components::Color^ color)
{
	RAYLIB::DrawTexture(texture, position->x, position->y, color->toNative());
}

void GraphicsWrapper::GL_UnloadModel(RAYLIB::Model& model)
{
	RAYLIB::UnloadModel(model);
}

RAYLIB::Shader& GraphicsWrapper::GL_LoadShaderFromMemory(String^ vertexShaderCode, String^ fragmentShaderCode)
{
	return RAYLIB::LoadShaderFromMemory(CastStringToNative(vertexShaderCode).c_str(), CastStringToNative(fragmentShaderCode).c_str());
}

RAYLIB::Shader& GraphicsWrapper::GL_LoadShader(String^ vertexShader, String^ fragmentShader)
{
	return RAYLIB::LoadShader(CastStringToNative(vertexShader).c_str(), CastStringToNative(fragmentShader).c_str());
}

RAYLIB::Texture& GraphicsWrapper::GL_LoadTexture(String^ texturePath)
{
	return RAYLIB::LoadTexture(CastStringToNative(texturePath).c_str());
}

void GraphicsWrapper::GL_UnloadTexture(RAYLIB::Texture& texture)
{
	RAYLIB::UnloadTexture(texture);
}

void GraphicsWrapper::GL_UnloadRenderTexture(RAYLIB::RenderTexture& renderTexture)
{
	RAYLIB::UnloadRenderTexture(renderTexture);
}

void GraphicsWrapper::GL_BeginDrawing()
{
	RAYLIB::BeginDrawing();
}

void GraphicsWrapper::GL_EndDrawing()
{
	RAYLIB::EndDrawing();
}

void GraphicsWrapper::GL_ClearBackground(Engine::Components::Color^ color)
{
	RAYLIB::ClearBackground(color->toNative());
}

void GraphicsWrapper::GL_DrawFPS(Engine::Components::Vector2^ position)
{
	RAYLIB::DrawFPS(position->x, position->y);
}

void GraphicsWrapper::GL_BeginBlendMode(rlWrapper::BlendMode mode)
{
	RAYLIB::BeginBlendMode((int)mode);
}

void GraphicsWrapper::GL_EndBlendMode()
{
	RAYLIB::EndBlendMode();
}

void GraphicsWrapper::GL_DrawRectangleLines(Engine::Components::Vector2^ position, Engine::Components::Vector2^ scale, Engine::Components::Color^ color)
{
	RAYLIB::DrawRectangleLines(position->x, position->y, scale->x, scale->y, color->toNative());
}

void GraphicsWrapper::GL_DrawRectangle(int x, int y, int width, int height, Engine::Components::Color^ color)
{
	RAYLIB::DrawRectangle(x, y, width, height, color->toNative());
}

void GraphicsWrapper::GL_DrawRectangleV(Engine::Components::Vector2^ position, Engine::Components::Vector2^ scale, Engine::Components::Color^ color)
{
	RAYLIB::DrawRectangleV(position->toNative(), scale->toNative(), color->toNative());
}

void GraphicsWrapper::GL_DrawLine(Engine::Components::Vector2^ origin, Engine::Components::Vector2^ dest, Engine::Components::Color^ color)
{
	RAYLIB::DrawLine(origin->x, origin->y, dest->x, dest->y, color->toNative());
}

GLWrapper::Texture2D GraphicsWrapper::ConvertTexture2D(RAYLIB::Texture2D& texture)
{
	GLWrapper::Texture2D newTexture = GLWrapper::Texture2D();

	newTexture.Id = texture.id;
	newTexture.Width = texture.width;
	newTexture.Height = texture.height;
	newTexture.Mipmaps = texture.mipmaps;
	newTexture.Format = (GLWrapper::PixelFormat)texture.format;

	return newTexture;
}

GLWrapper::RenderTexture2D GraphicsWrapper::ConvertRenderTexture2D(RAYLIB::RenderTexture2D& texture)
{
	GLWrapper::RenderTexture2D newTexture = GLWrapper::RenderTexture2D();

	newTexture.Id = texture.id;
	newTexture.Texture = ConvertTexture2D(texture.texture);
	newTexture.Depth = ConvertTexture2D(texture.depth);

	return newTexture;
}

RAYLIB::Texture2D& GraphicsWrapper::GL_GetTexture2D(unsigned int id)
{
	return Engine::Assets::Storage::DataPacks::singleton().GetTexture2D(id);
}

RAYLIB::Music& GraphicsWrapper::GL_GetMusic(unsigned int id)
{
	return Engine::Assets::Storage::DataPacks::singleton().GetMusic(id);
}

RAYLIB::Sound& GraphicsWrapper::GL_GetSound(unsigned int id)
{
	return Engine::Assets::Storage::DataPacks::singleton().GetSound(id);
}

RAYLIB::Model& GraphicsWrapper::GL_GetModel(unsigned int id)
{
	return Engine::Assets::Storage::DataPacks::singleton().GetModel(id);
}

RAYLIB::Shader& GraphicsWrapper::GL_GetShader(unsigned int id)
{
	return Engine::Assets::Storage::DataPacks::singleton().GetShader(id);
}