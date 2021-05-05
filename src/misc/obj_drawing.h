#ifndef CGL_UTIL_OBJDRAWING_H
#define CGL_UTIL_OBJDRAWING_H

#include <vector>
#include <iostream>
#include <fstream>

#include <nanogui/nanogui.h>

#include "CGL/CGL.h"

using namespace nanogui;

namespace CGL {
namespace Misc {
  
class ObjMesh {
public:
  // File location for .obj 
  ObjMesh(char const* fname);

  /**
   * Draws a sphere with the given position and radius in opengl, using the
   * current modelview/projection matrices and color/material settings.
   */
  void draw_obj(GLShader &shader, const Vector3D &p, double r);
  void loadOBJ(const char* filename);

private:
  const char* filename;
  
  MatrixXf positions;
  MatrixXf normals;
  MatrixXf uvs;
  MatrixXf tangents;

  int size = 0;
};


} // namespace Misc
} // namespace CGL

#endif // CGL_UTIL_SPHEREDRAWING_H
