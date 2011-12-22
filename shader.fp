#version 330 core
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec4 out_color;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;

#define SHADOW_MAP_SIZE 1024.0

#define MAX_LIGHTS 8
uniform vec3 light_ambient;
uniform vec3 player_pos;
uniform vec3 lights_pos[MAX_LIGHTS];
uniform vec3 lights_color[MAX_LIGHTS];
uniform ivec2 viewport_size;
uniform int lights_count;
layout(binding = 0) uniform sampler2D texture;
layout(binding = 1) uniform sampler2D shadow_texture0;

vec3 colorconv(vec3 c)
{
   return smoothstep(0.0, 1.0, c);
}

vec3 apply_light(vec3 pos, vec3 color, float diffuse_coeff, float specular_coeff)
{
   vec3 distance = model_vector - pos;
   vec3 light_direction = normalize(distance);

   float distance_correction = inversesqrt(dot(distance, distance));

   // Specular
   vec3 eye_vec = normalize(player_pos - model_vector);
   vec3 reflected = reflect(light_direction, normal);
   vec3 specular = pow(colorconv(specular_coeff * dot(eye_vec, reflected) * color * distance_correction), vec3(1.5));

   // Diffuse
   vec3 diffuse = colorconv(diffuse_coeff * dot(normal, -light_direction) * color * distance_correction);

   return specular + diffuse;
}

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
   vec4 tex = texture2D(texture, tex_coord);
   if (tex.a < 0.5)
      discard;

   vec3 result0 = lights_count >= 1 ? apply_light(lights_pos[0], lights_color[0], 30.0, 12.0) : vec3(0.0);
   vec2 shadow = vec2(gl_FragCoord.xy) / vec2(viewport_size);

   float shadow_factor0 = 0.0;
   float filt_max = 0.0;
   for (int i = -2; i <= 2; i++)
   {
      for (int j = -2; j <= 2; j++)
      {
         float filt = gaussian[(i + 2) * 5 + (j + 2)];
         shadow_factor0 += filt * texture2D(shadow_texture0, shadow + vec2(i, j) / SHADOW_MAP_SIZE).r;
         filt_max += filt;
      }
   }

   shadow_factor0 /= filt_max;

   out_color = vec4(tex.rgb * (light_ambient +
      result0 * shadow_factor0),
      tex.a);
}

