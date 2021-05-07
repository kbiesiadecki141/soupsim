#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with spheres.
  // 1. Compute where the point mass should have intersected the sphere
  //    by extending the path between its position and the sphere's origin
  //    to the sphere's surface. (tangent point)
  Vector3D dir = pm.position - origin;
  double p2s_radius = dir.norm2();
  if (p2s_radius < radius2) {
    // 2. Compute the correction vector to apply to last_position in
    //    order to reach the tangent point.
    dir.normalize();
    Vector3D correction = origin + dir * radius; // ray eqn
    correction -= pm.last_position; 

    // 3. Update position to be last position adjusted by correction
    //    and scaled by 1-f.
    pm.position = (1.0-friction) * correction + pm.last_position;
  }
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}

int Sphere::get_id() {
  return 3;
}
