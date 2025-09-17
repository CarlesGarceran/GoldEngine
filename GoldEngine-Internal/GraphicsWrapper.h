#ifdef _NULL

#pragma once

namespace Engine::Internal
{
    namespace rlWrapper
    {
        public value struct Texture
        {
        public:
            unsigned int id;
            int width;
            int height;
            int mipmaps;
            int format;

        public:
            static rlWrapper::Texture FromUnmanaged(RAYLIB::Texture unmanagedTexture)
            {
                rlWrapper::Texture tex;

                tex.id = unmanagedTexture.id;
                tex.width = unmanagedTexture.width;
                tex.height = unmanagedTexture.height;
                tex.mipmaps = unmanagedTexture.mipmaps;
                tex.format = unmanagedTexture.format;

                return tex;
            }

            static RAYLIB::Texture ApplyToUnmanaged(RAYLIB::Texture unmanagedRef, rlWrapper::Texture texture)
            {
                unmanagedRef.id = texture.id;
                unmanagedRef.width = texture.width;
                unmanagedRef.height = texture.height;
                unmanagedRef.mipmaps = texture.mipmaps;
                unmanagedRef.format = texture.format;

                return unmanagedRef;
            }
        };

        public value struct Color
        {
        public:
            unsigned char r, g, b, a;

            static rlWrapper::Color FromUnmanaged(RAYLIB::Color color)
            {
                rlWrapper::Color tmpColor;

                tmpColor.r = color.r;
                tmpColor.g = color.g;
                tmpColor.b = color.b;
                tmpColor.a = color.a;

                return tmpColor;
            }

            static RAYLIB::Color ApplyToUnmanaged(RAYLIB::Color unmanagedColor, rlWrapper::Color managedColor)
            {
                unmanagedColor.r = managedColor.r;
                unmanagedColor.g = managedColor.g;
                unmanagedColor.b = managedColor.b;
                unmanagedColor.a = managedColor.a;

                return unmanagedColor;
            }
        };

        public value struct MaterialMap
        {
        public:
            Texture texture;
            Color color;
            float value;


            static rlWrapper::MaterialMap FromUnmanaged(RAYLIB::MaterialMap materialMap)
            {
                rlWrapper::MaterialMap mmap;

                mmap.color = Color::FromUnmanaged(materialMap.color);
                mmap.texture = Texture::FromUnmanaged(materialMap.texture);
                mmap.value = materialMap.value;

                return mmap;
            }

            static RAYLIB::MaterialMap ApplyToUnmanaged(RAYLIB::MaterialMap unmanagedRef, rlWrapper::MaterialMap managedRef)
            {
                unmanagedRef.color = Color::ApplyToUnmanaged(unmanagedRef.color, managedRef.color);
                unmanagedRef.texture = Texture::ApplyToUnmanaged(unmanagedRef.texture, managedRef.texture);
                unmanagedRef.value = managedRef.value;

                return unmanagedRef;
            }
        };

        public value struct Matrix
        {
        public:
            float m0, m4, m8, m12;
            float m1, m5, m9, m13;
            float m2, m6, m10, m14;
            float m3, m7, m11, m15;


        public:
            static rlWrapper::Matrix FromUnmanaged(RAYLIB::Matrix unmanagedMatrix)
            {
                rlWrapper::Matrix matrix;

                matrix.m0 = unmanagedMatrix.m0;
                matrix.m1 = unmanagedMatrix.m1;
                matrix.m2 = unmanagedMatrix.m2;
                matrix.m3 = unmanagedMatrix.m3;
                matrix.m4 = unmanagedMatrix.m4;
                matrix.m5 = unmanagedMatrix.m5;
                matrix.m6 = unmanagedMatrix.m6;
                matrix.m7 = unmanagedMatrix.m7;
                matrix.m8 = unmanagedMatrix.m8;
                matrix.m9 = unmanagedMatrix.m9;
                matrix.m10 = unmanagedMatrix.m10;
                matrix.m11 = unmanagedMatrix.m11;
                matrix.m12 = unmanagedMatrix.m12;
                matrix.m13 = unmanagedMatrix.m13;
                matrix.m14 = unmanagedMatrix.m14;
                matrix.m15 = unmanagedMatrix.m15;

                return matrix;
            }

            static RAYLIB::Matrix ApplyToUnmanaged(RAYLIB::Matrix unmanagedMatrix, rlWrapper::Matrix managedMatrix)
            {
                unmanagedMatrix.m0 = managedMatrix.m0;
                unmanagedMatrix.m1 = managedMatrix.m1;
                unmanagedMatrix.m2 = managedMatrix.m2;
                unmanagedMatrix.m3 = managedMatrix.m3;
                unmanagedMatrix.m4 = managedMatrix.m4;
                unmanagedMatrix.m5 = managedMatrix.m5;
                unmanagedMatrix.m6 = managedMatrix.m6;
                unmanagedMatrix.m7 = managedMatrix.m7;
                unmanagedMatrix.m8 = managedMatrix.m8;
                unmanagedMatrix.m9 = managedMatrix.m9;
                unmanagedMatrix.m10 = managedMatrix.m10;
                unmanagedMatrix.m11 = managedMatrix.m11;
                unmanagedMatrix.m12 = managedMatrix.m12;
                unmanagedMatrix.m13 = managedMatrix.m13;
                unmanagedMatrix.m14 = managedMatrix.m14;
                unmanagedMatrix.m15 = managedMatrix.m15;

                return unmanagedMatrix;
            }
        };

        public value struct Mesh
        {
            int vertexCount;        // Number of vertices stored in arrays
            int triangleCount;      // Number of triangles stored (indexed or not)

            // Vertex attributes data
            float* vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
            float* texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
            float* texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
            float* normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
            float* tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
            unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
            unsigned short* indices;    // Vertex indices (in case vertex data comes indexed)

            // Animation vertex data
            float* animVertices;    // Animated vertex positions (after bones transformations)
            float* animNormals;     // Animated normals (after bones transformations)
            unsigned char* boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
            float* boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)

            // OpenGL identifiers
            unsigned int vaoId;     // OpenGL Vertex Array Object id
            unsigned int* vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)


        public:
            static rlWrapper::Mesh FromUnmanaged(RAYLIB::Mesh unmanagedMatrix)
            {
                rlWrapper::Mesh matrix;

                matrix.vertexCount = unmanagedMatrix.vertexCount;
                matrix.triangleCount = unmanagedMatrix.triangleCount;
                matrix.vertices = unmanagedMatrix.vertices;
                matrix.texcoords = unmanagedMatrix.texcoords;
                matrix.texcoords2 = unmanagedMatrix.texcoords2;
                matrix.normals = unmanagedMatrix.normals;
                matrix.tangents = unmanagedMatrix.tangents;
                matrix.colors = unmanagedMatrix.colors;
                matrix.indices = unmanagedMatrix.indices;
                matrix.animVertices = unmanagedMatrix.animVertices;
                matrix.animNormals = unmanagedMatrix.animNormals;
                matrix.boneIds = unmanagedMatrix.boneIds;
                matrix.boneWeights = unmanagedMatrix.boneWeights;
                matrix.vaoId = unmanagedMatrix.vaoId;
                matrix.vboId = unmanagedMatrix.vboId;

                return matrix;
            }

            static RAYLIB::Mesh ApplyToUnmanaged(RAYLIB::Mesh matrix, rlWrapper::Mesh unmanagedMatrix)
            {
                matrix.vertexCount = unmanagedMatrix.vertexCount;
                matrix.triangleCount = unmanagedMatrix.triangleCount;
                matrix.vertices = unmanagedMatrix.vertices;
                matrix.texcoords = unmanagedMatrix.texcoords;
                matrix.texcoords2 = unmanagedMatrix.texcoords2;
                matrix.normals = unmanagedMatrix.normals;
                matrix.tangents = unmanagedMatrix.tangents;
                matrix.colors = unmanagedMatrix.colors;
                matrix.indices = unmanagedMatrix.indices;
                matrix.animVertices = unmanagedMatrix.animVertices;
                matrix.animNormals = unmanagedMatrix.animNormals;
                matrix.boneIds = unmanagedMatrix.boneIds;
                matrix.boneWeights = unmanagedMatrix.boneWeights;
                matrix.vaoId = unmanagedMatrix.vaoId;
                matrix.vboId = unmanagedMatrix.vboId;

                return matrix;
            }
        };

        public value struct Shader
        {
        public:
            unsigned int id;
            int* locs;

            static rlWrapper::Shader FromUnmanaged(RAYLIB::Shader unmanagedTexture)
            {
                rlWrapper::Shader shader;

                shader.id = unmanagedTexture.id;
                shader.locs = unmanagedTexture.locs;

                return shader;
            }

            static RAYLIB::Shader ApplyToUnmanaged(RAYLIB::Shader originalTexture, rlWrapper::Shader managedTexture)
            {
                originalTexture.id = managedTexture.id;
                originalTexture.locs = managedTexture.locs;

                return originalTexture;
            }
        };

        public value struct Model
        {
        public:
            Matrix transform;

            int meshCount;
            int materialCount;
            Mesh* meshes;
            // materials
            int* meshMaterial;

            int boneCount;
        };

        public value struct RenderTexture
        {
        public:
            unsigned int id;
            rlWrapper::Texture texture;
            rlWrapper::Texture depth;


            static RAYLIB::Texture convertToNative(rlWrapper::Texture tex)
            {
                RAYLIB::Texture texture = { 0 };

                rlWrapper::Texture::ApplyToUnmanaged(texture, tex);

                return texture;
            }

            static rlWrapper::RenderTexture FromUnmanaged(RAYLIB::RenderTexture unmanagedTexture)
            {
                rlWrapper::RenderTexture renderTexture;

                renderTexture.id = unmanagedTexture.id;
                renderTexture.texture = rlWrapper::Texture::FromUnmanaged(unmanagedTexture.texture);
                renderTexture.depth = rlWrapper::Texture::FromUnmanaged(unmanagedTexture.depth);

                return renderTexture;
            }

            static RAYLIB::RenderTexture ApplyToUnmanaged(RAYLIB::RenderTexture originalTexture, rlWrapper::RenderTexture managedTexture)
            {
                originalTexture.id = managedTexture.id;
                rlWrapper::Texture::ApplyToUnmanaged(originalTexture.texture, managedTexture.texture);
                rlWrapper::Texture::ApplyToUnmanaged(originalTexture.depth, managedTexture.depth);

                return originalTexture;
            }
        };

        public enum class ShaderLocs
        {
            SHADER_LOC_VERTEX_POSITION = 0, // Shader location: vertex attribute: position
            SHADER_LOC_VERTEX_TEXCOORD01,   // Shader location: vertex attribute: texcoord01
            SHADER_LOC_VERTEX_TEXCOORD02,   // Shader location: vertex attribute: texcoord02
            SHADER_LOC_VERTEX_NORMAL,       // Shader location: vertex attribute: normal
            SHADER_LOC_VERTEX_TANGENT,      // Shader location: vertex attribute: tangent
            SHADER_LOC_VERTEX_COLOR,        // Shader location: vertex attribute: color
            SHADER_LOC_MATRIX_MVP,          // Shader location: matrix uniform: model-view-projection
            SHADER_LOC_MATRIX_VIEW,         // Shader location: matrix uniform: view ( Camera transform)
            SHADER_LOC_MATRIX_PROJECTION,   // Shader location: matrix uniform: projection
            SHADER_LOC_MATRIX_MODEL,        // Shader location: matrix uniform: model (transform)
            SHADER_LOC_MATRIX_NORMAL,       // Shader location: matrix uniform: normal
            SHADER_LOC_VECTOR_VIEW,         // Shader location: vector uniform: view
            SHADER_LOC_COLOR_DIFFUSE,       // Shader location: vector uniform: diffuse color
            SHADER_LOC_COLOR_SPECULAR,      // Shader location: vector uniform: specular color
            SHADER_LOC_COLOR_AMBIENT,       // Shader location: vector uniform: ambient color
            SHADER_LOC_MAP_ALBEDO,          // Shader location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
            SHADER_LOC_MAP_METALNESS,       // Shader location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
            SHADER_LOC_MAP_NORMAL,          // Shader location: sampler2d texture: normal
            SHADER_LOC_MAP_ROUGHNESS,       // Shader location: sampler2d texture: roughness
            SHADER_LOC_MAP_OCCLUSION,       // Shader location: sampler2d texture: occlusion
            SHADER_LOC_MAP_EMISSION,        // Shader location: sampler2d texture: emission
            SHADER_LOC_MAP_HEIGHT,          // Shader location: sampler2d texture: height
            SHADER_LOC_MAP_CUBEMAP,         // Shader location: samplerCube texture: cubemap
            SHADER_LOC_MAP_IRRADIANCE,      // Shader location: samplerCube texture: irradiance
            SHADER_LOC_MAP_PREFILTER,       // Shader location: samplerCube texture: prefilter
            SHADER_LOC_MAP_BRDF             // Shader location: sampler2d texture: brdf
        };

        public enum class UniformType
        {
            SHADER_UNIFORM_FLOAT = 0,       // Shader uniform type: float
            SHADER_UNIFORM_VEC2,            // Shader uniform type: vec2 (2 float)
            SHADER_UNIFORM_VEC3,            // Shader uniform type: vec3 (3 float)
            SHADER_UNIFORM_VEC4,            // Shader uniform type: vec4 (4 float)
            SHADER_UNIFORM_INT,             // Shader uniform type: int
            SHADER_UNIFORM_IVEC2,           // Shader uniform type: ivec2 (2 int)
            SHADER_UNIFORM_IVEC3,           // Shader uniform type: ivec3 (3 int)
            SHADER_UNIFORM_IVEC4,           // Shader uniform type: ivec4 (4 int)
            SHADER_UNIFORM_SAMPLER2D        // Shader uniform type: sampler2d
        };

        public enum class BlendMode {
            BLEND_ALPHA = 0,
            BLEND_ADDITIVE,
            BLEND_MULTIPLIED,
            BLEND_ADD_COLORS,
            BLEND_SUBTRACT_COLORS,
            BLEND_ALPHA_PREMULTIPLY,
            BLEND_CUSTOM,
            BLEND_CUSTOM_SEPARATE
        };

        public value struct Material
        {
        public:
            Shader shader;
            MaterialMap* maps;
            float* params;

            static rlWrapper::Material FromUnmanaged(RAYLIB::Material unmanagedRef)
            {
                rlWrapper::Material managedRef;

                managedRef.shader = Shader::FromUnmanaged(unmanagedRef.shader);
                managedRef.maps = &MaterialMap::FromUnmanaged(*unmanagedRef.maps);
                managedRef.params = unmanagedRef.params;

                return managedRef;
            }
        };
    }


    /// <summary>
    /// This class its not memory safe.
    /// All the methods are raw and the user is expected to manage the memory by its own. Allocating and deallocating resources when needed.
    /// </summary>
    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
    public ref class GraphicsWrapper abstract
    {
    public:

        /* -- SETTERS -- */

static void GL_SetShaderValue(RAYLIB::Shader&, int, void*, rlWrapper::UniformType);
static void GL_SetShaderValue(RAYLIB::Shader&, rlWrapper::ShaderLocs, void*, rlWrapper::UniformType);

/*-- MODES --*/

static void GL_BeginDrawing();
static void GL_BeginTextureMode(RAYLIB::RenderTexture);
static void GL_BeginShaderMode(RAYLIB::Shader);
static void GL_BeginBlendMode(rlWrapper::BlendMode);

static void GL_EndTextureMode();
static void GL_EndShaderMode();
static void GL_EndDrawing();
static void GL_EndBlendMode();

/* -- LOADERS -- */

static RAYLIB::Model& GL_LoadModel(String^);
static RAYLIB::RenderTexture& GL_LoadRenderTexture(int, int);
static RAYLIB::Texture& GL_LoadTexture(String^);
static RAYLIB::Shader& GL_LoadShader(String^, String^);
static RAYLIB::Shader& GL_LoadShaderFromMemory(String^, String^);
static void GL_LoadDepthTexture(RAYLIB::RenderTexture&, int, int, bool);

/* -- GETTERS -- */

static int  GL_GetShaderLoc(RAYLIB::Shader&, const char*);
static RAYLIB::RenderTexture& GL_GetDepthTexture(int, int);

/* -- GRAPHICS -- */

static void GL_DrawLine(Engine::Components::Vector2, Engine::Components::Vector2, Engine::Components::Color^);
static void GL_DrawRectangleLines(Engine::Components::Vector2, Engine::Components::Vector2, Engine::Components::Color^);
static void GL_DrawRectangle(int, int, int, int, Engine::Components::Color^);
static void GL_DrawRectangleV(Engine::Components::Vector2, Engine::Components::Vector2, Engine::Components::Color^);
static void GL_ClearBackground(Engine::Components::Color^);
static void GL_DrawModel(RAYLIB::Model&, Engine::Components::Vector3, float, Engine::Components::Color^);
static void GL_DrawTexture(RAYLIB::Texture&, Engine::Components::Vector2, Engine::Components::Color^);
static void GL_DrawFPS(Engine::Components::Vector2);

/* -- UNLOADERS -- */
static void GL_UnloadTexture(RAYLIB::Texture&);
static void GL_UnloadModel(RAYLIB::Model&);
static void GL_UnloadRenderTexture(RAYLIB::RenderTexture&);


static GLWrapper::Texture2D ConvertTexture2D(RAYLIB::Texture2D& texture);
static GLWrapper::RenderTexture2D ConvertRenderTexture2D(RAYLIB::RenderTexture2D& texture);

/* -- DATAPACK ENGINE -- */

static RAYLIB::Texture2D& GL_GetTexture2D(unsigned int id);
static RAYLIB::Music& GL_GetMusic(unsigned int id);
static RAYLIB::Sound& GL_GetSound(unsigned int id);
static RAYLIB::Model& GL_GetModel(unsigned int id);
static RAYLIB::Shader& GL_GetShader(unsigned int id);
    };
}

#endif