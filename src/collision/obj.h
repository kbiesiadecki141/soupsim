#ifndef COLLISIONOBJECT_OBJ_H
#define COLLISIONOBJECT_OBJ_H

#include "../clothMesh.h"
#include "../misc/obj_drawing.h"
#include "collisionObject.h"

using namespace CGL;
using namespace std;

struct Obj : public CollisionObject {
public:
  Obj(const Vector3D &origin, double radius, double friction, const char* filename)
      : origin(origin), radius(radius), friction(friction), m_obj_mesh(Misc::ObjMesh(filename)) {}

  void render(GLShader &shader);
  void collide(PointMass &pm);
  int get_id();


private:
  Vector3D origin;
  double radius;

  double friction;
  
  Misc::ObjMesh m_obj_mesh;
};

#endif /* COLLISIONOBJECT_OBJ_H */
