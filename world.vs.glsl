#version 330

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_color;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_world;

out vec2 f_texcoord;
out vec3 f_normal;
out vec3 f_color;
out vec3 f_position;

void main() {
   gl_Position = u_projection * u_view * u_world * vec4(a_position, 1.0);

   f_color = a_color;
   f_normal = mat3(u_world) * a_normal;
   f_texcoord = a_texcoord;
}