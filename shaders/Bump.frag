#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  return 0.0;
}

void main() {
  // YOUR CODE HERE
  kh = u_texture_2.height;
  kn = u_texture_2_size.

  du = (h(u + 1/w, v) - h(u, v)) * kh * kn;
  dv = (h(u, v+1/h) - h(u, v) * kh * kn);

  vec3 n = v_normal.xyz;
  vec3 b =

  vec4 no = (-du, -dv, 1);
  nd =

  // (Placeholder code. You will want to replace it.)
  out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  out_color.a = 1;
}
