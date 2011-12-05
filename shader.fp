#version 330 core

out vec4 FragColor;
in vec3 normal;
in vec3 model_vector;
in vec2 tex_coord;
uniform sampler2D texture;

#define MAX_LIGHTS 8
uniform int lights_count;
uniform vec3 lights_pos[MAX_LIGHTS];
uniform vec3 lights_color[MAX_LIGHTS];
uniform vec3 light_ambient;

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
   vec3 specular = colorconv(specular_coeff * dot(eye_vec, reflected) * color * distance_correction);

   // Diffuse
   vec3 diffuse = colorconv(diffuse_coeff * dot(normal, -light_direction) * color * distance_correction);

   return specular + diffuse;
}

void main()
{
   vec3 result = light_ambient;

   vec4 tex = texture2D(texture, tex_coord);
   int count = min(MAX_LIGHTS, lights_count);
   for (int i = 0; i < count; i++)
      result += apply_light(lights_pos[i], lights_color[i], 20.0, 5.0);

   FragColor = vec4(tex.rgb * result, tex.a);
}
