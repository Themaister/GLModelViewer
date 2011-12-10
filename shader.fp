#version 330 core

layout(location = 0) out vec4 out_color;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;
in vec3 shadow;

#define MAX_LIGHTS 8
layout(std140) uniform Lights
{
   vec3 light_ambient;
   vec3 lights_pos[MAX_LIGHTS];
   vec3 lights_color[MAX_LIGHTS];
   int lights_count;
};
uniform sampler2D texture;
uniform sampler2DShadow shadow_texture;

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
   vec3 eye_vec = normalize(-model_vector);
   vec3 reflected = reflect(light_direction, normal);
   vec3 specular = smoothstep(0.0, 1.0, pow(dot(eye_vec, reflected), 2.0)) * color * distance_correction;

   // Diffuse
   vec3 diffuse = colorconv(diffuse_coeff * dot(normal, -light_direction) * color * distance_correction);

   return specular + diffuse;
}

#define DECL_SHADOW(i, x, y) const ivec2 shadow_offset##i = ivec2(x, y)
#define LOOKUP(i) shadow_factor += (shadow.z <= textureOffset(shadow_texture, shadow, shadow_offset##i) + 0.0001) ? (1.0 / 17.0) : 0.000

// textureOffset needs compile-time constants.
DECL_SHADOW(0, 0, 1);   DECL_SHADOW(1, 1, 1);
DECL_SHADOW(2, -1, 1);  DECL_SHADOW(3, 0, -1);
DECL_SHADOW(4, 1, -1);  DECL_SHADOW(5, -1, -1);
DECL_SHADOW(6, 0, 0);   DECL_SHADOW(7, 1, 0);
DECL_SHADOW(8, -1, 0);  DECL_SHADOW(9, 2, 2);
DECL_SHADOW(10, -2, 2); DECL_SHADOW(11, 0, -2);
DECL_SHADOW(12, 2, -2); DECL_SHADOW(13, -1, -2);
DECL_SHADOW(14, 2, 0);  DECL_SHADOW(15, -2, 0);
DECL_SHADOW(16, 0, 2);

void main()
{
   vec4 tex = texture2D(texture, tex_coord);
   if (tex.a < 0.5) // Alpha test
      discard;

   vec3 result = vec3(0.0);
   int count = min(MAX_LIGHTS, lights_count);
   
   for (int i = 0; i < count; i++)
      result += apply_light(lights_pos[i], lights_color[i], 20.0, 15.0);

   float shadow_factor = 0.0;
   LOOKUP(0); LOOKUP(1); LOOKUP(2); LOOKUP(3);
   LOOKUP(4); LOOKUP(5); LOOKUP(6); LOOKUP(7);
   LOOKUP(8); LOOKUP(9); LOOKUP(10); LOOKUP(11);
   LOOKUP(12); LOOKUP(13); LOOKUP(14); LOOKUP(15); LOOKUP(16);

   out_color = vec4(tex.rgb * (light_ambient + result * shadow_factor), tex.a);
}

