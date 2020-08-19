#version 450
#extension GL_ARB_separate_shader_objects : enable

// warning: some datatypes use multiple slots and the location needs to be increased by 2
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

// uniforms
layout(binding = 0) uniform UBO {
    mat4 M;
    mat4 V;
    mat4 P;
} ubo;

void main() {
    gl_Position = ubo.P * ubo.V * ubo.M * vec4(position, 1.0);
    fragColor = color;
    fragUV = uv;
}
