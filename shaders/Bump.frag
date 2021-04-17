#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_4;
uniform vec2 u_texture_4_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  return texture(u_texture_4, uv)[0];
}

void main() {
  // YOUR CODE HERE

  float u = v_uv[0];
  float v0 = v_uv[1];
  float w = u_texture_4_size[0];
  float height = u_texture_4_size[1];

  float kh = u_height_scaling;
  float kn = u_normal_scaling;

  vec2 arg1 = vec2(u + 1.0/w, v0);
  vec2 arg2 = vec2(u, v0 + 1/height);
  float du = (h(arg1) - h(v_uv)) * kh * kn;
  float dv = (h(arg2) - h(v_uv)) * kh * kn;

  vec3 b = cross(v_normal.xyz, v_tangent.xyz);
  mat3 tbn = mat3(v_tangent.xyz, b, v_normal.xyz);

  vec3 no = vec3(-du, -dv, 1);
  vec3 nd = normalize(tbn * no);

  vec3 l = u_light_pos - v_position.xyz;
  vec3 v = normalize(u_cam_pos - v_position.xyz);
  vec3 height1 = (v + normalize(l)) / sqrt(dot((v + normalize(l)), (v + normalize(l))));
  vec3 r_squared = l * l;
  vec4 diffuse = vec4(u_color.xyz * (u_light_intensity / (dot(l, l))) * max(0, dot(normalize(nd.xyz), normalize(l))), 1.0);
  vec4 spec = vec4(.5 * (u_light_intensity / (dot(l, l))) * pow(max(0, dot(normalize(nd.xyz), normalize(height1))), 100), 1.0);
  vec4 ia = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 ambient = .1 * ia;

  out_color = diffuse + ambient + spec;

}
