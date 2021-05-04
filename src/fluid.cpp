#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "fluid.h"
#include "spring.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Fluid::Fluid(double width, double height, double depth, int num_width_points,
             int num_height_points, int num_depth_points) {
    this->width = width;
    this->height = height;
    this->num_width_points = num_width_points;
    this->num_height_points = num_height_points;

    buildFluid();
    //buildFluidMesh();
}

Fluid::~Fluid() {
    particles.clear();

    if (fluidMesh) {
        delete fluidMesh;
    }
}

void Fluid::buildFluid() {
  float min_i = 0;
  float min_j = 0;
  float min_k = 0;
  float max_i = width;
  float max_j = height;
  float max_k = depth;
  float delta_i = (max_i - min_i)/num_width_points;
  float delta_j = (max_j - min_j)/num_height_points;
  float delta_k = (max_k - min_k)/num_depth_points;

  for (int j = 0; j < num_height_points; j++) {
    for (int i = 0; i < num_width_points; i++) {
      //for (int k = 0; k < num_depth_points; k++) {
        Vector3D position;
        if (orientation == HORIZONTAL) {
          // set y coordinate to 1 for all point masses while varying x,z
          position[0] = i*delta_i;
          position[1] = 1;
          position[2] = j*delta_j;
        } else { // == VERTICAL
          position[0] = i*delta_i;
          position[1] = j*delta_j;
          position[2] = 1;
        }

        // the pinned vector stores the INDICES!! of pinned masses - not the positions!
        // if inside pinned vector, pinned = true!
        bool pin = false;
        for (vector<int> v : pinned) {
          if (v[0] == i && v[1] == j) {
            pin = true;
            continue;
          }
        }

        particles.push_back(Particle(position, pin));
      //} // end k
    } // end i 
  } // end j

  // POPULATE GRID WITH SPRINGS
  // =======================================================
  //for (int z = 0; z < num_depth_points; z++) {
    for (int y = 0; y < num_height_points; y++) {
      for (int x = 0; x < num_width_points; x++) {
        Particle * pm = &particles[y * num_width_points + x];
        //Particle * pm = &particles[x + width * (y + depth * z)]; // index into flat 3D array

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

        } // end if 
      } // end x
    } // end y
  //} // end z

  return;
}

float Fluid::hash_position(Vector3D pos) {

    // hash position for a fluid (might not work too well since particles fly around the place)
    // TODO: Replace this mechanism with FLANN!
    double w = 3 * width / num_width_points;
    double h = 3 * height / num_height_points;
    double d = 3 * depth / num_depth_points;

    int cx = pos.x / w;
    int cy = pos.y / h;
    int cz = pos.z / d;

    // Hash function
    return (cx << 10 ^ cy << 5 ^ cz);
}

bool Fluid::indexInRange(int x, int y) {
    return x >= 0 && x < num_width_points && y >= 0 && y < num_height_points;
}

int Fluid::flatten(int x, int y) {
    return x * num_width_points + y;
}

void Fluid::simulate(double frames_per_sec, double simulation_steps, FluidParameters *fp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
    // TODO: Utilize FLANN to find neighbouring particles given any particle - Thomas
    // TODO: Compute physics of moving the particles - Katelyn

    double mass = width * height * depth * fp->density / num_width_points / num_height_points / num_depth_points;
    double delta_t = 1.0f / frames_per_sec / simulation_steps;

    // Compute total force acting on each point mass.
    // Reset all forces to 0
    clear_forces();

    // Gravity or other external forces
    Vector3D total_external_accelerations;
    for (int i = 0; i < external_accelerations.size(); i++) {
        total_external_accelerations += external_accelerations[i];
    }
    for (auto p = particles.begin(); p != particles.end(); p++) {
        p->forces += total_external_accelerations * mass;
    }


    // TODO: Apply incompressibility correction

    // TODO: Apply artificial pressure
    Vector3D delta_q = Vector3D(0.03, 0.03, 0.03);
    double n = 4.0;

    // TODO: apply viscosity

    // TODO: apply vorticity

    // TODO Use Verlet integration to apply forces (IDK WHERE THIS GOES)
    for (auto &point : particles) {
        double d = fp->damping / 100.0;
        Vector3D last_position = point.position;
        point.position = point.position + (1.0 - d) * (point.position - point.last_position) + (point.forces/mass) * delta_t*delta_t;
        point.last_position = last_position;
    }
}

void Fluid::clear_forces() {
    for (auto p = particles.begin(); p != particles.end(); p++) {
        p->forces = Vector3D();
    }
}

void Fluid::build_spatial_map() {
    for (const auto &entry : map) {
        delete(entry.second);
    }
    map.clear();


    // TODO (Part 4): Build a spatial map out of all of the point masses.
    for (auto &point : particles) {
        float pointHash = hash_position(point.position);

        // if 0, allocate space for new vector
        if (map[pointHash] == NULL) {
            map[pointHash] = new vector<Particle*>();
        }
        // add to vector
        map[pointHash]->push_back(&point);
    }


}

void Fluid::self_collide(Particle &p, double simulation_steps) {
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Fluid::reset() {
    Particle *p = &particles[0];
    for (int i = 0; i < particles.size(); i++) {
        p->position = p->start_position;
        p->last_position = p->start_position;
        p++;
    }
}
