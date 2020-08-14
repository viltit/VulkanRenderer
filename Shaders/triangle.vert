#version 450
#extension GL_ARB_separate_shader_objects : enable

// warning: some datatypes use multiple slots and the location needs to be increased by 2
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location=0) out vec3 fragColor;

// uniforms
layout(binding = 0) uniform UBO {
    mat4 MVP;
} ubo;

void main() {
    gl_Position = ubo.MVP * vec4(position, 1.0);
    fragColor = color;
}
