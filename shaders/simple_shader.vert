#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec2 fragTexCoord;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
} ubo;

layout (push_constant) uniform Push {
    layout(offset = 0) mat4 model;
} push;

void main() {
    gl_Position = ubo.projection * ubo.view * push.model * vec4(position, 0.0, 1.0);

    fragTexCoord = texCoord;
}