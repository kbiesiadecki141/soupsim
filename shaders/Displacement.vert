#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

<<<<<<< HEAD
uniform sampler2D u_texture_3;
uniform vec2 u_texture_3_size;
=======
uniform sampler2D u_texture_4;
uniform vec2 u_texture_4_size;
>>>>>>> 2b2fdbf63f36c6cd3d45e46a836ad6da02408413

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 in_position;
in vec4 in_normal;
in vec4 in_tangent;
in vec2 in_uv;

out vec4 v_position;
out vec4 v_normal;
out vec2 v_uv;
out vec4 v_tangent;

float h(vec2 uv) {
  // You may want to use this helper function...
<<<<<<< HEAD
  return texture(u_texture_3, uv)[0];
=======
  return texture(u_texture_4, uv)[0];
>>>>>>> 2b2fdbf63f36c6cd3d45e46a836ad6da02408413
}

void main() {
  // YOUR CODE HERE

  float kh = u_height_scaling;

  // (Placeholder code. You will want to replace it.)
  v_position = u_model * in_position;
  v_normal = normalize(u_model * in_normal);
  v_uv = in_uv;
  v_tangent = normalize(u_model * in_tangent);

  v_position = v_position + v_normal * h(v_uv) * kh;

  gl_Position = u_view_projection * u_model * v_position;
}
