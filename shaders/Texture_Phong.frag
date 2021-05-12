#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform sampler2D u_texture_2;
uniform sampler2D u_texture_1;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
    // YOUR CODE HERE
    out_color = texture(u_texture_1, v_uv);
    vec3 l = u_light_pos - v_position.xyz;
    vec3 v = normalize(u_cam_pos - v_position.xyz);
    vec3 h = (v + normalize(l)) / sqrt(dot((v + normalize(l)), (v + normalize(l))));
    vec3 r_squared = l * l;
    vec4 diffuse = vec4(u_color.xyz * (u_light_intensity / (dot(l, l))) * max(0, dot(normalize(v_normal.xyz), normalize(l))), 1.0);
    vec4 spec = vec4(.5 * (u_light_intensity / (dot(l, l))) * pow(max(0, dot(normalize(v_normal.xyz), normalize(h))), 100), 1.0);
    vec4 ia = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 ambient = .1 * ia;
    out_color += (0.2) * (diffuse);
    out_color.a = 1.0;
}