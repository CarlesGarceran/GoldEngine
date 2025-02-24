#include "../../SDK.h"
#include "../../Objects/Pipeline/ScriptableRenderPipeline.hpp"
#include "ACES_Tonemapper.h"


using namespace Engine::Render::BuiltIn;

const char* vertexShader = R"(
#version 430

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
)";


const char* fragmentShader = R"(
#version 430

in vec2 fragTexCoord;

layout(location = 0)
uniform sampler2D texture0;

uniform float u_exposure = 1;

out vec4 finalColor;

mat3x3 ACESInputMat = mat3x3
(
	0.59719, 0.35458, 0.04823,
	0.07600, 0.90834, 0.01566,
	0.02840, 0.13383, 0.83777
);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
mat3x3 ACESOutputMat = mat3x3
(
	 1.60475, -0.53108, -0.07367,
	-0.10208,  1.10813, -0.00605,
	-0.00327, -0.07276,  1.07602
);

vec3 RRTAndODTFit(vec3 v)
{
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
	vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
}

vec3 ACESFitted(vec3 color)
{
	color = transpose(ACESInputMat) * color;
	// Apply RRT and ODT
	color = RRTAndODTFit(color);
	color = transpose(ACESOutputMat) * color;
	color = clamp(color, 0, 1);
	return color;
}

vec3 ACESFilmSimple(vec3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0, 1);

}

vec4 rgb_to_lmsr(vec3 c)
{
	const mat4x3 m = mat4x3(
		0.31670331, 0.70299344, 0.08120592, 
		0.10129085, 0.72118661, 0.12041039, 
		0.01451538, 0.05643031, 0.53416779, 
		0.01724063, 0.60147464, 0.40056206);
	return c * m;
}
vec3 lms_to_rgb(vec3 c)
{
	const mat3 m = mat3(
		 4.57829597, -4.48749114,  0.31554848, 
		-0.63342362,  2.03236026, -0.36183302, 
		-0.05749394, -0.09275939,  1.90172089);
	return c * m;
}

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    color.rgb = ACESFitted(color.rgb * u_exposure);
    finalColor = vec4(pow(color.rgb, vec3(1/2.2)), color.a);
}
)";

void onUnloadShader(Shader& shader)
{
	UnloadShader(shader);
}

ACES_Tonemapper::ACES_Tonemapper()
{
	RAYLIB::Shader shader = LoadShaderFromMemory(vertexShader, fragmentShader);
	
	shader.locs[0] = RAYLIB::GetShaderLocation(shader, "texture0");
	shader.locs[1] = RAYLIB::GetShaderLocation(shader, "u_exposure");

	tonemapper = new Engine::Native::EnginePtr<RAYLIB::Shader>(shader, &onUnloadShader, &onUnloadShader);

	SharedInstance::Create<float>("SE_ACES_Exposure", 2.0f);
}

bool ACES_Tonemapper::ManualRendering() 
{
	return false; // DISABLE MANUAL RENDERING 
	// The rendering is done to a texture that will be overriden per effect, creating a render stack (post processing)
	// By toggling this option in 'false' the engine will automatically render the previous frame (raw processed or post processed) using OnEffectBegin and OnEffectEnd, allowing you to use custom shaders, render things over it.
	// If this option is in 'true' you must use OnEffectApply, and you'll get the entire framebuffer from it. you'll need to do the rendering manually otherwise the frames won't be rendered
	// RECOMMENDATION: If you're a begginer with GLSL / Native C++ or C# code, don't turn this to true, let it remain on false.
}

void ACES_Tonemapper::OnEffectBegin()
{
	RAYLIB::BeginShaderMode(tonemapper->getInstance());
}

void ACES_Tonemapper::OnEffectEnd()
{
	RAYLIB::EndShaderMode();
}

void ACES_Tonemapper::OnEffectUnload()
{
	delete tonemapper;
}

void ACES_Tonemapper::SetTexture(RAYLIB::Texture* texturePtr)
{
	float exposure = 1.0f;

	if (SharedInstance::ExistsInstance("SE_ACES_Exposure"))
		exposure = SharedInstance::Get<float>("SE_ACES_Exposure");

	RAYLIB::SetShaderValueTexture(tonemapper->getInstance(), RAYLIB::GetShaderLocation(tonemapper->getInstance(), "texture0"), *texturePtr);
	RAYLIB::SetShaderValue(tonemapper->getInstance(), RAYLIB::GetShaderLocation(tonemapper->getInstance(), "u_exposure"), &exposure, SHADER_UNIFORM_FLOAT);
}