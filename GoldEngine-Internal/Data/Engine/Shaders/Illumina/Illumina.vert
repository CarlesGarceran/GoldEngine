#version 330
#define MAX_BONE_NUM 128

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec4 vertexColor;
in vec4 vertexBoneIds;
in vec4 vertexBoneWeights;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform vec3 lightPos;
uniform vec4 difColor;
uniform mat4 boneMatrices[MAX_BONE_NUM];

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out mat3 TBN;

const float normalOffset = 0.1;

bool matrixIsEmpty(mat4 matrix[MAX_BONE_NUM])
{
    for (int x = 0; x < MAX_BONE_NUM; x++)
    {
        for(int i = 0; i < 4; i++)
        {
            if(any(matrix[x][i] != vec4(0)))
            {
                return false;
            }
        }
    }

    return true;
}


void main()
{
    // Compute binormal from vertex normal and tangent
    vec3 vertexBinormal = cross(vertexNormal, vertexTangent);
    
    // Compute fragment normal based on normal transformations
    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    
    // Compute fragment position based on model transformations
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0f));

    fragTexCoord = vertexTexCoord*2.0;
    fragNormal = normalize(normalMatrix*vertexNormal);
    vec3 fragTangent = normalize(normalMatrix*vertexTangent);
    fragTangent = normalize(fragTangent - dot(fragTangent, fragNormal)*fragNormal);
    vec3 fragBinormal = normalize(normalMatrix*vertexBinormal);
    fragBinormal = cross(fragNormal, fragTangent);

    TBN = transpose(mat3(fragTangent, fragBinormal, fragNormal));

    if(!matrixIsEmpty(boneMatrices))
    {
        int boneIndex0 = int(vertexBoneIds.x);
        int boneIndex1 = int(vertexBoneIds.y);
        int boneIndex2 = int(vertexBoneIds.z);
        int boneIndex3 = int(vertexBoneIds.w);

        vec4 skinnedPosition =
            vertexBoneWeights.x*(boneMatrices[boneIndex0]*vec4(vertexPosition, 1.0)) +
            vertexBoneWeights.y*(boneMatrices[boneIndex1]*vec4(vertexPosition, 1.0)) + 
            vertexBoneWeights.z*(boneMatrices[boneIndex2]*vec4(vertexPosition, 1.0)) + 
            vertexBoneWeights.w*(boneMatrices[boneIndex3]*vec4(vertexPosition, 1.0));

        vec4 skinnedNormal =
            vertexBoneWeights.x*(boneMatrices[boneIndex0]*vec4(vertexNormal, 0.0)) +
            vertexBoneWeights.y*(boneMatrices[boneIndex1]*vec4(vertexNormal, 0.0)) + 
            vertexBoneWeights.z*(boneMatrices[boneIndex2]*vec4(vertexNormal, 0.0)) + 
            vertexBoneWeights.w*(boneMatrices[boneIndex3]*vec4(vertexNormal, 0.0));
        skinnedNormal.w = 0.0;

        fragNormal = normalize(mat3(matNormal) * skinnedNormal.xyz);

        gl_Position = mvp*skinnedPosition;
    }
    else
    {
        gl_Position = mvp*vec4(vertexPosition, 1.0);
    }
}