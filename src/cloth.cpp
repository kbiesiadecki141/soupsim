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
  //buildCircle();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildCircle() {
  // TODO (Part 1): Build a grid of masses and springs.
  // Evenly-spaced grid over (0,0):(width, height)
  // row-major order! i.e. vector[j * width + i] for (i, j)

  // POPULATE GRID WITH MASSES
  // =======================================================
  // assert width == height
  if (width != height) { return; }

  float min_i = 0;
  float min_j = 0;
  float max_i = width;
  float max_j = height;
  float delta_i = (max_i - min_i)/num_width_points;
  float delta_j = (max_j - min_j)/num_height_points;

  Vector3D center = Vector3D(0,1,0);
  float radius = 0.5*width;

  getchar();
  for (int j = 0; j < num_height_points; j++) {
    for (int i = 0; i < num_width_points; i++) {
      Vector3D position;
      if (orientation == HORIZONTAL) {
        // set y coordinate to 1 for all point masses while varying x,z

        position[0] = i*delta_i;
        position[1] = 1;
        position[2] = j*delta_j;

        float r2 = pow(radius,2);

        float inside_circle = pow(position[0]-center.x, 2) + pow(position[1]-center.y, 2);

        // perimeter of circle is pinned
        bool pin = false;
        if (abs(inside_circle - r2) < 0.1) { pin = true; }

        if (inside_circle < r2) {
          point_masses.push_back(PointMass(position, pin));
        }

      } else { // == VERTICAL
        position[0] = i*delta_i;
        position[1] = j*delta_j;

        // generate a small random offset --> z
        // https://stackoverflow.com/questions/686353/random-float-number-generation#:~:text=In%20modern%20c%2B%2B%20you,and%20distribution%20to%20be%20static%20.
        float HI =  1.0/1000;
        float LO = -1.0/1000;
        position[2] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

        point_masses.push_back(PointMass(position, true));
      }
    }
  }


  // POPULATE GRID WITH SPRINGS
  // =======================================================
  int count = 0;
  for (auto &pm : point_masses) {
      // TODO: Add a check to see if the pm is pinned - then, ignore constraint

      Vector3D pos = pm.position;
      
      // structural: one above and one left
      if (count < point_masses.size()) 
          springs.push_back(Spring(&pm, &pm + 1, STRUCTURAL));
      count++; 

      /*
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
      */
  }
  return;
}

bool Cloth::isCorner(int i, int j, int num_height_points, int num_width_points) {
    return false;
    return (i == 0 and j == 0) \
        or (i == num_width_points - 1 and j == 0) \
        or (i == 0 and j == num_height_points - 1) \
        or (i == num_width_points - 1 and j == num_height_points - 1);
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  // Evenly-spaced grid over (0,0):(width, height)
  // row-major order! i.e. vector[j * width + i] for (i, j)

  // POPULATE GRID WITH MASSES
  // =======================================================

  float min_i = 0;
  float min_j = 0;
  float max_i = 1;
  float max_j = 1;
  float delta_i = (max_i - min_i)/num_width_points;
  float delta_j = (max_j - min_j)/num_height_points;

  float radius = 0.47;

  for (int j = 0; j < num_height_points; j++) {
    for (int i = 0; i < num_width_points; i++) {
      Vector3D position;
      if (orientation == HORIZONTAL) {
        // set y coordinate to 1 for all point masses while varying x,z
      position[0] = i*delta_i;
      if (isCorner(i, j, num_height_points, num_width_points)) {
          position[1] = 0.9;
      } else {
          position[1] = 1;
      }
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
      bool pin = false;
      float r2 = pow(radius,2);
      Vector3D center = Vector3D(0.5,1,0.5);
    

      float inside_circle = pow(position[0]-center.x, 2) + pow(position[2]-center.z, 2);

      // perimeter of circle and four corners are pinned
      if (abs(inside_circle - r2) < 0.01 or isCorner(i, j, num_height_points, num_width_points)) {
         //getchar();
         pin = true; 
      }

      point_masses.push_back(PointMass(position, pin));
    }
  }

  // POPULATE GRID WITH SPRINGS
  // =======================================================
  for (int y = 0; y < num_height_points; y++) {
    for (int x = 0; x < num_width_points; x++) {
        // sometimes, don't populate with a spring to get clumps in the soup
        if (rand() % 10 == 1) {
            continue;
        }

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
  Vector3D ext_force = Vector3D();
  for (Vector3D a : external_accelerations) {
    ext_force += a * mass;
  }

  for (PointMass &p : point_masses) {
    p.forces = ext_force;
  }

  for (Spring s: springs) {
    if ((cp->enable_structural_constraints && s.spring_type == STRUCTURAL)
      || (cp->enable_shearing_constraints && s.spring_type == SHEARING)
      || (cp->enable_bending_constraints && s.spring_type == BENDING)) { // run if enabled

      float bend = s.spring_type == BENDING ? .2 : 1.0;

      Vector3D direction = (s.pm_b->position - s.pm_a->position);
      direction.normalize();

      Vector3D spring_force = (bend * cp->ks) * (abs((s.pm_a->position - s.pm_b->position).norm()) - s.rest_length) * direction;

      s.pm_a->forces += spring_force;
      s.pm_b->forces += -spring_force;
    }
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (PointMass &p : point_masses) {
    if (!p.pinned) {
      Vector3D pos = p.position + (1.0 - (cp->damping / 100)) * (p.position - p.last_position) + p.forces / mass * delta_t * delta_t;
      p.last_position = p.position;
      p.position = pos;
    }
  }

  // TODO (Part 4): Handle self-collisions.

  // TODO (Part 3): Handle collisions with other primitives.
  // FIXME combine loop with other parts
  for (auto &p : point_masses) {
    for (auto *co : *collision_objects) {
      //co->collide(p);
    }
  }

  build_spatial_map();
  for (auto &p : point_masses) {
    self_collide(p, simulation_steps);
  }   

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].

// For each spring, apply this constraint by correcting the two point masses' positions
// such that the spring's length is at most 10% greater than its rest_length
// at the end of any time step. Maintain the same vector direction between the two point masses
// and only modify their distance apart from each other. Perform half of the correction to
// each point mass, unless one of them is pinned, in which case apply the correction
// entirely to one point mass. Do nothing if both are pinned (they wouldn't satisfy
// the condition anyways, by construction; can you see why?).

  for (Spring s : springs) {
    if ((abs((s.pm_a->position - s.pm_b->position).norm()) > s.rest_length * 1.1) && !(s.pm_a->pinned && s.pm_b->pinned)) {

      Vector3D direction = (s.pm_a->position - s.pm_b->position);
      direction.normalize();

      // calculate how far out the point has overshot
      auto change = abs((abs((s.pm_a->position - s.pm_b->position).norm()) - s.rest_length * 1.1)) * direction;

      if (s.pm_a->pinned) {
        s.pm_b->position += change;
      } else if (s.pm_b->pinned) {
        s.pm_a->position -= change;
      } else {
        s.pm_b->position += change * .5;
        s.pm_a->position -= change * .5;
      }
    }
  }
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
    vector<PointMass *>* v = new vector<PointMass *>();
    if (map[hash] == NULL) {
      map[hash] = v;
    }
    v = map[hash];
    v->push_back(&p);
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  vector<PointMass *> *v = map[hash_position(pm.position)];
  Vector3D correction = Vector3D(0, 0, 0);
  int count = 0;
  for (auto *p : *v) {
    if (p == &pm) {
      continue;
    }
  
// two aspects: for each candidate oint pass, 
// shoud e in the direction that points away from the
// point mass. once you apply it to the position,
// it should brig you 2*thickness from candidate mass and point mass
// lenght bringing you to 2.0*thickness

    // im being a idiot sad
    // correction applied to the point mass (pm) not p
    // want distance tween pm and p to e 2*thickness
    Vector3D dir = p->position-pm.position; 
    if (dir.norm() <= 2.0*thickness) { // is this within range?
      correction += (2.0*thickness - dir.norm())*dir;

      count++;
    }
  }

  // the final correction vector is the average of all these 
  // pairwise correction vectors scales down by sim_steps
  if (count > 0) {
    correction /= count; // average it out
    pm.position = pm.position + correction/simulation_steps; // scale down
  }
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.

  float w = 3 * width / num_width_points;
  float h = 3 * height / num_height_points;
  float t = max(w, h);

  Vector3D new_pos;
  new_pos.x = floor(pos.x / w);
  new_pos.y = floor(pos.y / h);
  new_pos.z = floor(pos.z / t);

  return new_pos.x * 31 + new_pos.y * 37 + new_pos.z * 41;
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
