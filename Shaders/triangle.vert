#version 450
#extension GL_ARB_separate_shader_objects : enable

// warning: some datatypes use multiple slots and the location needs to be increased by 2
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;     // TODO: Obsolete
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragView;
layout(location = 4) out vec3 fragLight;

// uniforms
layout(binding = 0) uniform UBO {
    mat4 M;
    mat4 V;
    mat4 P;
    vec3 lightPos;    // TODO: Give point light a range
} ubo;

void main() {
    gl_Position = ubo.P * ubo.V * ubo.M * vec4(position, 1.0);

    vec4 worldPos = ubo.M * vec4(position, 1.f);
    fragNormal =  mat3(ubo.V) * mat3(ubo.M) * normal;
    fragView = vec3(ubo.V * worldPos);
    fragLight = mat3(ubo.V) * (ubo.lightPos - vec3(worldPos));

    fragColor = color;
    fragUV = uv;
}
