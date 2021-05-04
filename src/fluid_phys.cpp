#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "fluid.h"
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

    // Make particles and ordered so that it is stored in row Major order?
    for (int h = 0; h < num_height_points; h += 1) {
        for (int x = 0; x < num_width_points; x += 1) {
            for (int d = 0; d < num_depth_points; d += 1) {
                // TODO: Build fluid box full of particles - Amy
            }
        }
    }
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
