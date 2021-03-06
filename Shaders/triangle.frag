#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragView;
layout(location = 4) in vec3 fragLight;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

// TODO: Rather bad use case for a push constant, different shading should happen in different shaders
layout(push_constant) uniform PushConstant {
    bool debugNormals;
} pushConstant;

void main() {

    // outColor = vec4(fragColor, 1.0);
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLight);
    vec3 V = normalize(fragView);

    // reflect light on fragments normal
    vec3 R = reflect(L, N);

    vec3 fragColor = vec3(texture(tex, fragUV));
    vec3 ambient = 0.1 * fragColor;                         // TODO: Set ambient factor as uniform
    vec3 diffuse = 0.5 * max(dot(N, L), 0) * fragColor;     // TODO: Again, do not hardcode factor

    // TODO: Use a specular texture
    vec3 specular = pow(max(dot(R, V), 0), 16)  * vec3(1.f);             // TODO: 16 ...
    if (pushConstant.debugNormals) {
        /*
        To debug our normal and view vector, we can take their dot product. If view and normal are aligned, it is 1, if they are
        perpendicular, it is 0, if they are in opposite, it is -1
        --> we should not see any black faces. Faces that look at us directly must be white
        */
        float val = dot(N, V);
        outColor = vec4(val, val, val, 1.f);
    }
    else {
        outColor = vec4(ambient + diffuse + specular, 1.f);
    }

}
