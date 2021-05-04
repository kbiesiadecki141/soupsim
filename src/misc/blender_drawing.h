#ifndef CGL_UTIL_BLENDERDRAWING_H
#define CGL_UTIL_BLENDERDRAWING_H

#include <vector>

#include <nanogui/nanogui.h>

#include "CGL/CGL.h"

using namespace nanogui;

namespace CGL {
namespace Misc {
  
class ObjMesh {
public:
  // File location for .obj 
  ObjMesh(const char* filename);
  
  /**
   * Draws a sphere with the given position and radius in opengl, using the
   * current modelview/projection matrices and color/material settings.
   */
  void draw_obj(GLShader &shader, const Vector3D &p, double r);

private:
  std::vector<unsigned int> Indices;
  std::vector<double> Vertices;
  
  int s_index(int x, int y);
  
  void build_data();
  
  int obj_num_vertices;
  int obj_num_indices;
  
  MatrixXf positions;
  MatrixXf normals;
  MatrixXf uvs;
  MatrixXf tangents;
};


} // namespace Misc
} // namespace CGL

#endif // CGL_UTIL_SPHEREDRAWING_H
