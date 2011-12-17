#version 330 core

layout(location = 0) out vec4 out_color;
in vec4 shadow;

layout(binding = 1) uniform sampler2DShadow shadow_texture;

void main()
{
   float f = shadow2DProj(shadow_texture, shadow - vec4(0.0, 0.0, 0.01, 0.0)).r;
   out_color = vec4(f, f, f, 1.0);
}

