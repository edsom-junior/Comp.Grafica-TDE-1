#version 330 core
in vec3 surfaceNormal;
in vec2 texCoord;
out vec4 fragColor;
uniform vec4 Color;
uniform bool isTextured;
uniform sampler2D Texture0;
uniform float tiling;
void main()
{
    vec4 base = isTextured ? texture(Texture0, texCoord * tiling) : Color;
    float light = 0.55 + 0.45 * max(dot(normalize(surfaceNormal), normalize(vec3(-0.5, 1.0, 0.8))), 0.0);
    fragColor = vec4(base.rgb * light, base.a);
}
