#include <iostream>
#include <math.h>
#include <random>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  // Evenly-spaced grid over (0,0):(width, height)
  // row-major order! i.e. vector[j * width + i] for (i, j) 

  // POPULATE GRID WITH MASSES
  // =======================================================
  float min_i = 0;
  float min_j = 0;
  float max_i = width;
  float max_j = height;
  float delta_i = (max_i - min_i)/num_width_points; 
  float delta_j = (max_j - min_j)/num_height_points; 

  for (int j = 0; j < num_height_points; j++) {
    for (int i = 0; i < num_width_points; i++) {
      Vector3D position;
      if (orientation == HORIZONTAL) {
        // set y coordinate to 1 for all point masses while varying x,z
        position[0] = i*delta_i;   
        position[1] = 1;   
        position[2] = j*delta_j;   
      } else { // == VERTICAL
        position[0] = i*delta_i;   
        position[1] = j*delta_j;   

        // generate a small random offset --> z
        // https://stackoverflow.com/questions/686353/random-float-number-generation#:~:text=In%20modern%20c%2B%2B%20you,and%20distribution%20to%20be%20static%20.
        float HI =  1.0/1000;
        float LO = -1.0/1000;
        //srand (static_cast <unsigned> (time(0))); // random seed
        position[2] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

      }

      // the pinned vector stores the INDICES!! of pinned masses - not the positions!
      // if inside pinned vector, pinned = true!
      std::vector<int> xy;
      xy.push_back(position[1]);
      xy.push_back(position[0]);
      bool pin = std::find(pinned.begin(), pinned.end(), xy) != pinned.end();
      point_masses.push_back(PointMass(position, pin));
    }
  }

  // POPULATE GRID WITH SPRINGS
  // =======================================================
  for (int y = 0; y < num_height_points; y++) {
    for (int x = 0; x < num_width_points; x++) {
      // is this actually the easier way?
      PointMass * pm = &point_masses[y * num_width_points + x];

      if (y > 0) {
        springs.push_back(Spring(pm, pm-num_width_points, STRUCTURAL)); // one above
        if (x+1 < num_width_points) {
          springs.push_back(Spring(pm, pm-num_width_points+1, SHEARING)); // diagonal right
        }
        if (y-2 >= 0) {
          springs.push_back(Spring(pm, pm -2*num_width_points, BENDING)); // two above
        }
      }

      if (x-1 >= 0) {
        springs.push_back(Spring(pm, pm-1, STRUCTURAL)); // left 
        if (y-1 >= 0) {
          springs.push_back(Spring(pm, pm-num_width_points-1, SHEARING)); // diagonal left
        }
        if (x-2 >= 0) {
          springs.push_back(Spring(pm, pm-2, BENDING)); // two left
        }
        
      }
    }
  } 

  return;
}
/*
 * This function runs one time step of the cloth simulation.
 */
void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
  // external forces + spring correction forces
  // point_mass.forces= external_accelerations + sum(ks(||pa-pb|| - l))

  for (auto &p : point_masses) {
    // 0. Clear/reset forces
    // perhaps call delete(p.forces) ?
    p.forces = Vector3D(0,0,0);

    // 1. Compute total f_ext (F = ma)
    for (Vector3D a : external_accelerations) {
      p.forces += mass * a;
    } 
  }
     
   // 2. Apply spring correction forces. (equal+opposite)
  for (auto &s : springs) {
    if (s.spring_type == STRUCTURAL && !cp->enable_structural_constraints) {
      Vector3D f = cp->ks * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);
      s.pm_a->forces += f;
      s.pm_b->forces -= f;
    }
    if (s.spring_type == SHEARING && !cp->enable_shearing_constraints) {
      Vector3D f = cp->ks * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);
      s.pm_a->forces += f;
      s.pm_b->forces -= f;
    }
    if (s.spring_type == BENDING && !cp->enable_bending_constraints) {
      // bending constraint should be weaker than the above ^
      Vector3D f = 0.2*cp->ks * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);
      s.pm_a->forces += f;
      s.pm_b->forces -= f;
    }
   }
    
    
  
    
    
  
    // TODO (Part 2): Use Verlet integration to compute new point mass positions
  
  
    // TODO (Part 4): Handle self-collisions.
  
  
    // TODO (Part 3): Handle collisions with other primitives.
    // FIXME combine loop with other parts
    for (auto &p : point_masses) {
      self_collide(p, simulation_steps);
      for (auto *co : *collision_objects) {
        co->collide(p);
      }
    }
  
  
    // TODO (Part 2): Constrain the changes to be such that the spring does not change
    // in length more than 10% per timestep [Provot 1995].
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  // unordered_map<float, vector<PointMass *> *>
  for (auto &p : point_masses) {
    float hash = hash_position(p.position);
    std::vector<PointMass *> v = *(map[hash]);
    v.push_back(&p);
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  Vector3D correction = Vector3D(0, 0, 0);
  for (auto p : point_masses) {
    if (p.position == pm.position) {
      continue;
    }
    float r = (p.position-pm.position).norm();
    if (r < 2*thickness) {
      correction += p.position + r * (2.0*thickness);
      correction -= pm.position;
    }
  }
  // the final correction vector is the average of all these 
  // pairwise correction vectors scales down by sim_steps
  correction /= simulation_steps;
  pm.position = pm.position + correction;
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
  float w = 3 * width / num_width_points;
  float h = 3 * height / num_height_points;
  float t = max(w, h);

  Vector3D new_pos;
  new_pos.x = fmod(pos.x, w);  
  new_pos.y = fmod(pos.y, h);  
  new_pos.z = fmod(pos.z, t);  
  
  // https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates 
  // note: x, y, z are constrained by (0, 1) so this should never overflow
  return (new_pos.x * 31) + (new_pos.y * 37) + (new_pos.z * 41); 
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
