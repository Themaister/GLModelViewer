#version 330 core

layout(location = 0) out vec4 out_color;
in vec4 shadow;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;

#define MAX_LIGHTS 8
uniform vec3 light_ambient;
uniform vec3 player_pos;
uniform vec3 lights_pos[MAX_LIGHTS];
uniform vec3 lights_color[MAX_LIGHTS];
uniform int lights_count;
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
   if (tex.a < 0.5) // Alpha test
      discard;

   vec3 result = vec3(0.0);
   int count = min(MAX_LIGHTS, lights_count);
   
   for (int i = 0; i < count; i++)
      result += apply_light(lights_pos[i], lights_color[i], 30.0, 12.0);

   float shadow_factor = shadow.z / shadow.w < shadow2DProj(shadow_texture, shadow) + 0.00005 ? 1.0 : 0.0;
   out_color = vec4(tex.rgb * (light_ambient + result * shadow_factor), tex.a);
}

