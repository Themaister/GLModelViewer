#version 420 core

layout(location = 0) out vec4 out_color;
in vec4 shadow;

#define SHADOW_MAP_SIZE 2048.0

layout(binding = 1) uniform sampler2DShadow shadow_texture;

void main()
{
   float f = 0.0;
   for (int i = -1; i <= 1; i++)
      for (int j = -1; j <= 1; j++)
         f += textureProj(shadow_texture, shadow + vec4(shadow.w * vec2(i, j) / SHADOW_MAP_SIZE, -0.0001 * shadow.w, 0.0));

   f /= 9.0;
   out_color = vec4(f, f, f, 1.0);
}

