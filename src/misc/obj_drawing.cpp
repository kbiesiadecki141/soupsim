#include <cmath>
#include <nanogui/nanogui.h>

#include "obj_drawing.h"
#include <glm/glm.hpp>

#include "CGL/color.h"
#include "CGL/vector3D.h"

#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5
#define TANGEN_OFFSET 8
#define VERTEX_SIZE 11

using namespace nanogui;

namespace CGL {
namespace Misc {

ObjMesh::ObjMesh(char const* fname) : filename(fname) {

  loadOBJ(filename);
}

void ObjMesh::loadOBJ(const char* filename) {
  // Vertex portions
  std::vector<glm::fvec3> vertex_positions;
  std::vector<glm::fvec2> vertex_texcoords;
  std::vector<glm::fvec3> vertex_normals;

  // Face vectors: hold index to each of these ^
  std::vector<GLint> vertex_position_indices;
  std::vector<GLint> vertex_texcoord_indices;
  std::vector<GLint> vertex_normal_indices;

  // Vertex array
  std::stringstream ss;
  std::ifstream in_file(filename);
  std::string line = "";
  std::string prefix = "";
  glm::vec3 temp_vec3;
  glm::vec2 temp_vec2;
  GLint temp_glint = 0;

  // file error
  if (!in_file.is_open()) {
    throw "ERROR: Can't open file.";
  }

  // read one line at a time
  while (std::getline(in_file, line)) {
    // get the prefix of the line
    ss.clear();
    ss.str(line);
    ss >> prefix;

    if (prefix == "#") {

    } else if (prefix == "o") {
    } else if (prefix == "s") {
    } else if (prefix == "use_mtl") {
    } else if (prefix == "f") { // faces
      int counter = 0;
      while (ss >> temp_glint) {
        if (counter == 0) {
          vertex_position_indices.push_back(temp_glint);
        } else if (counter == 1) {
          vertex_texcoord_indices.push_back(temp_glint);
        } else if (counter == 2) {
          vertex_normal_indices.push_back(temp_glint);
        }

        // handling characters
        if (ss.peek() == '/') {
          ++counter;

          ss.ignore(1, '/');
        } else if (ss.peek() == ' ') {
          ++counter;
          ss.ignore(1, ' ');
        }

        // reset counter
        if (counter > 2) {
          counter = 0;
        }
      }
    } else if (prefix == "v") { // vertex position
      ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
      vertex_positions.push_back(temp_vec3);
    } else if (prefix == "vt") { // vertex position
      ss >> temp_vec2.x >> temp_vec2.y;
      vertex_texcoords.push_back(temp_vec2);
    } else if (prefix == "vn") { // vertex normal
      ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
      vertex_normals.push_back(temp_vec3);
    } else {
    }

    // build mesh
    positions = MatrixXf(4, vertex_position_indices.size() * 3);
    normals = MatrixXf(4, vertex_position_indices.size() * 3);
    uvs = MatrixXf(2, vertex_position_indices.size() * 3);
    //tangents = MatrixXf(4, sphere_num_indices * 3);

    for (size_t i = 0; i < vertex_position_indices.size(); i++) {
      glm::vec3 pos = vertex_positions[vertex_position_indices[i]-1];
      positions.col(i) << pos.x, pos.y, pos.z, 1.0;

      glm::vec2 uv = vertex_texcoords[vertex_texcoord_indices[i]-1];
      uvs.col(i) << uv.x, uv.y;
      
      glm::vec3 nor = vertex_normals[vertex_normal_indices[i]-1]; 
      normals.col(i) << nor.x, nor.y, nor.z, 0.0;

      // color = glm::vec3(1.f, 1.f, 1.f);
    }

    // debug
    std::cout << line << "\n";
  }

  size = vertex_position_indices.size();
  std::cout << "OBJ file loaded!" << "\n";
}


void ObjMesh::draw_obj(GLShader &shader, const Vector3D &p, double r) {

  Matrix4f model;
  model << r, 0, 0, p.x, 0, r, 0, p.y, 0, 0, r, p.z, 0, 0, 0, 1;

  shader.setUniform("u_model", model);

  shader.uploadAttrib("in_position", positions);

  if (shader.attrib("in_normal", false) != -1) {
    shader.uploadAttrib("in_normal", normals);
  }
  if (shader.attrib("in_uv", false) != -1) {
    shader.uploadAttrib("in_uv", uvs);
  }
  
  // tengents?
  /*
  if (shader.attrib("in_tangent", false) != -1) {
    shader.uploadAttrib("in_tangent", tangents, false);
  }
  */

  shader.drawArray(GL_TRIANGLES, 0, size);
}

} // namespace Misc
} // namespace CGL