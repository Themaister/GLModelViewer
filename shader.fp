#version 330 core
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec4 out_color;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;

#define SHADOW_MAP_SIZE 2048.0

#define MAX_LIGHTS 8
uniform vec3 light_ambient;
uniform vec3 player_pos;
uniform vec3 lights_pos[MAX_LIGHTS];
uniform vec3 lights_color[MAX_LIGHTS];
uniform ivec2 viewport_size;
uniform int lights_count;
layout(binding = 0) uniform sampler2D texture;
layout(binding = 1) uniform sampler2D shadow_texture0;
layout(binding = 2) uniform sampler2D shadow_texture1;
layout(binding = 3) uniform sampler2D shadow_texture2;

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

void main()
{
   vec4 tex = texture2D(texture, tex_coord);

   vec3 result0 = lights_count >= 1 ? apply_light(lights_pos[0], lights_color[0], 30.0, 12.0) : vec3(0.0);
   vec3 result1 = lights_count >= 2 ? apply_light(lights_pos[1], lights_color[1], 30.0, 12.0) : vec3(0.0);
   vec3 result2 = lights_count >= 3 ? apply_light(lights_pos[2], lights_color[2], 30.0, 12.0) : vec3(0.0);

   vec2 shadow = vec2(gl_FragCoord.xy) / vec2(viewport_size);

   float shadow_factor0 = 0.0;
   float shadow_factor1 = 0.0;
   float shadow_factor2 = 0.0;
   for (int i = -1; i <= 1; i++)
      for (int j = -1; j <= 1; j++)
   {
      shadow_factor0 += texture2D(shadow_texture0, shadow + vec2(i, j) / SHADOW_MAP_SIZE).r;
      shadow_factor1 += texture2D(shadow_texture1, shadow + vec2(i, j) / SHADOW_MAP_SIZE).r;
      shadow_factor2 += texture2D(shadow_texture2, shadow + vec2(i, j) / SHADOW_MAP_SIZE).r;
   }

   shadow_factor0 /= 9.0;
   shadow_factor1 /= 9.0;

   out_color = vec4(tex.rgb * (light_ambient +
      result0 * shadow_factor0 +
      result1 * shadow_factor1 +
      result2 * shadow_factor2), tex.a);
}

