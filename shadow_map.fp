#version 330 core
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec4 out_color;
in vec4 shadow;

#define SHADOW_MAP_SIZE 1024.0

layout(binding = 1) uniform sampler2DShadow shadow_texture;

#define elem(x, y) exp(-sqrt(dot(vec2(x, y), vec2(x, y))))
#define row(n) elem(-2, n), elem(-1, n), elem(0, n), elem(1, n), elem(2, n)

const float gaussian[25] = {
   row(2),
   row(1),
   row(0),
   row(-1),
   row(-2),
};

void main()
{
   float f = 0.0;
   float filt_max = 0.0;
   for (int i = -2; i <= 2; i++)
   {
      for (int j = -2; j <= 2; j++)
      {
         float filt = gaussian[(i + 2) * 5 + (j + 2)];
         f += filt * textureProj(shadow_texture,
            shadow + vec4(shadow.w * vec2(i, j) / SHADOW_MAP_SIZE, -0.03, 0.0));
         filt_max += filt;
      }
   }

   f /= filt_max;
   out_color = vec4(f, f, f, 1.0);
}

