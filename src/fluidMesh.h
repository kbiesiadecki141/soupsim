#ifndef CLOTH_MESH_H
#define CLOTH_MESH_H

#include <vector>

#include "CGL/CGL.h"
#include "particle.h"

using namespace CGL;
using namespace std;
// TODO: Make fluid mesh?

class Triangle {
public:
  Triangle(Particle *pm1, Particle *pm2, Particle *pm3, Vector3D uv1, Vector3D uv2, Vector3D uv3)
      : pm1(pm1), pm2(pm2), pm3(pm3), uv1(uv1), uv2(uv2), uv3(uv3) {}

  // Static references to constituent mesh objects
  Particle *pm1;
  Particle *pm2;
  Particle *pm3;
  
  // UV values for each of the points.
  // Uses Vector3D for convenience. This means that the z dimension
  // is not used, and xy corresponds to uv.
  Vector3D uv1;
  Vector3D uv2;
  Vector3D uv3;

  Halfedge *halfedge;
}; // struct Triangle

class Edge {
public:
  Halfedge *halfedge;
}; // struct Edge

class Halfedge {
public:
  Edge *edge;
  Halfedge *next;
  Halfedge *twin;
  Triangle *triangle;
  Particle *pm;
}; // struct Halfedge

class FluidMesh {
public:
  ~FluidMesh() {}

  vector<Triangle *> triangles;
}; // struct FluidMesh

#endif // FLUID_MESH_H
