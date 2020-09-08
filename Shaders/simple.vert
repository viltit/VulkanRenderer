// very simple shader, best suited for debugging / drawing wireframes

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 uv;
layout(location = 0) out vec2 fragUV;

// uniforms
layout(binding = 0) uniform UBO {
    mat4 M;
    mat4 V;
    mat4 P;
} ubo;

void main() {
    gl_Position =  ubo.P * ubo.V * ubo.M * vec4(position, 1.0);
    fragUV = uv;
}
