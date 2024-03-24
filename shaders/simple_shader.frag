#version 450

layout (location = 0) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D texSampler;

layout (push_constant) uniform Push {
    layout(offset = 64) vec3 color;
    layout(offset = 80) float opacity;
} push;

void main() {
    // outColor = vec4(fragTexCoord, 0.0, 1.0);
    outColor = vec4(push.color, push.opacity) * texture(texSampler, fragTexCoord);
}