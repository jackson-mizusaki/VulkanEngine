#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 vTexCoord;
layout(location = 4) in vec4 vColor;
layout(location = 5) in uvec4 a_joint;
layout(location = 6) in vec4 a_weight;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;
layout(location = 2) out vec3 fragPosWorld;
layout(location = 3) out vec3 fragNormalWorld;

struct PointLight 
{
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUBO
{
	mat4 u_jointMat[2];
	mat4 projection;
	mat4 view;
	mat4 invView;
	vec4 ambientLightColor;
	PointLight pointLights[10]; // can be specialized constant
	int numLights;
} ubo;

layout(push_constant) uniform Push 
{ 
	mat4 modelMatrix; // projection * view * model
	mat4 normalMatrix;
	
} push;


void main()
{
    mat4 skinMat =
        a_weight.x * ubo.u_jointMat[int(a_joint.x)] +
        a_weight.y * ubo.u_jointMat[int(a_joint.y)] +
        a_weight.z * ubo.u_jointMat[int(a_joint.z)] +
        a_weight.w * ubo.u_jointMat[int(a_joint.w)];
	vec4 positionWorld = skinMat * push.modelMatrix * vec4(position, 1.0);
	vec4 cameraPosition = ubo.view * positionWorld;
	gl_Position = ubo.projection * cameraPosition;
	 
	 // nonuniform scaling
	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = positionWorld.xyz;
	fragColor = vColor.xyz;


}