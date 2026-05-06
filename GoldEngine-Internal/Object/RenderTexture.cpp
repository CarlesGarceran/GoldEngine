#include "../Includes.h"
#include "../GlIncludes.h"
#include "../EnginePtr.h"
#include "../LuaAPI.h"
#include "RenderTexture.h"

void UnloadRenderTextureDepthTex(RAYLIB::RenderTexture2D target)
{
	if (target.id > 0)
	{
		// Color texture attached to FBO is deleted
		RLGL::rlUnloadTexture(target.texture.id);
		RLGL::rlUnloadTexture(target.depth.id);

		// NOTE: Depth texture is automatically
		// queried and deleted before deleting framebuffer
		RLGL::rlUnloadFramebuffer(target.id);
	}
}

RAYLIB::RenderTexture2D LoadRenderTextureDepthTex(int width, int height)
{
	RAYLIB::RenderTexture2D target = { 0 };

	target.id = RLGL::rlLoadFramebuffer(); // Load an empty framebuffer

	if (target.id > 0)
	{
		RLGL::rlEnableFramebuffer(target.id);

		// Create color texture (default to RGBA)
		target.texture.id = RLGL::rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
		target.texture.width = width;
		target.texture.height = height;
		target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		target.texture.mipmaps = 1;

		// Create depth texture buffer (instead of raylib default renderbuffer)
		target.depth.id = RLGL::rlLoadTextureDepth(width, height, false);
		target.depth.width = width;
		target.depth.height = height;
		target.depth.format = PIXELFORMAT_UNCOMPRESSED_R32;
		target.depth.mipmaps = 1;

		// Attach color texture and depth texture to FBO
		RLGL::rlFramebufferAttach(target.id, target.texture.id, RLGL::RL_ATTACHMENT_COLOR_CHANNEL0, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);
		RLGL::rlFramebufferAttach(target.id, target.depth.id, RLGL::RL_ATTACHMENT_DEPTH, RLGL::RL_ATTACHMENT_TEXTURE2D, 0);

		// Check if fbo is complete with attachments (valid)
		if (RLGL::rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

		RLGL::rlDisableFramebuffer();
	}
	else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

	return target;
}

Engine::Components::RenderTexture::RenderTexture(int width, int height)
	: RenderTexture(width, height, false) { }

Engine::Components::RenderTexture::RenderTexture(int width, int height, bool attachDepth)
{
	if (attachDepth) 
	{
		this->resource = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(
			LoadRenderTextureDepthTex(width, height),
			UnloadRenderTextureDepthTex,
			UnloadRenderTextureDepthTex
		);
	}
	else
	{
		this->resource = new Engine::Native::EnginePtr<RAYLIB::RenderTexture>(
			RAYLIB::LoadRenderTexture(width, height),
			RAYLIB::UnloadRenderTexture,
			RAYLIB::UnloadRenderTexture
		);
	}
}

Engine::Components::RenderTexture::~RenderTexture()
{
	delete this->resource;
}

RAYLIB::RenderTexture& Engine::Components::RenderTexture::GetRenderTexture()
{
	return this->resource->getInstance();
}

RAYLIB::RenderTexture* Engine::Components::RenderTexture::GetRenderTexturePointer()
{
	return this->resource->getPointer();
}

int Engine::Components::RenderTexture::Width::get()
{
	return this->resource->getInstance().texture.width;
}

int Engine::Components::RenderTexture::Height::get()
{
	return this->resource->getInstance().texture.height;
}

RAYLIB::RenderTexture* Engine::Components::RenderTexture::Handle::get()
{
	return this->resource->getPointer();
}
