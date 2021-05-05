#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/obj_drawing.h"
#include "obj.h"

using namespace nanogui;
using namespace CGL;

void Obj::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with objs.
}

void Obj::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the obj when rendered
  m_obj_mesh.draw_obj(shader, origin, radius * 0.92);
}
