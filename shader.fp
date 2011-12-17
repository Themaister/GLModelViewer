#version 330 core

layout(location = 0) out vec4 out_color;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;

#define MAX_LIGHTS 8
uniform vec3 light_ambient;
uniform vec3 player_pos;
uniform vec3 lights_pos[MAX_LIGHTS];
uniform vec3 lights_color[MAX_LIGHTS];
uniform ivec2 viewport_size;
uniform int lights_count;
layout(binding = 0) uniform sampler2D texture;
layout(binding = 1) uniform sampler2D shadow_texture;

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

#define decl(deg) vec2(cos(2 * 3.1415 * deg / 8.0), sin(2 * 3.1415 * deg / 8.0))
const vec2 offsets[25] = {
   decl(0), decl(1), decl(2), decl(3),
   decl(4), decl(5), decl(6), decl(7)
};

void main()
{
   vec4 tex = texture2D(texture, tex_coord);

   vec3 result = vec3(0.0);
   int count = min(MAX_LIGHTS, lights_count);
   
   for (int i = 0; i < count; i++)
      result += apply_light(lights_pos[i], lights_color[i], 30.0, 12.0);

   vec2 shadow = vec2(gl_FragCoord.xy) / vec2(viewport_size);

   float shadow_factor = 0.0;
   for (int i = 0; i < 8; i++)
   {
      shadow_factor += 0.8 * texture2D(shadow_texture, shadow + offsets[i] / 8092.0).r;
      shadow_factor += 0.2 * texture2D(shadow_texture, shadow + offsets[i] / 4096.0).r;
      shadow_factor += 0.05 * texture2D(shadow_texture, shadow + offsets[i] / 2048.0).r;
   }

   shadow_factor /= (0.8 + 0.2 + 0.05) * 8;

   out_color = vec4(tex.rgb * (light_ambient + result * shadow_factor), tex.a);
}

