#ifndef FLUID_H
#define FLUID_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "collision/collisionObject.h"
#include "spring.h"

using namespace CGL;
using namespace std;

enum e_orientation { HORIZONTAL = 0, VERTICAL = 1 };

struct FluidParameters {
    FluidParameters() {}
    FluidParameters(double damping,
                    double density,
                    double viscosity)
            : damping(damping), density(density), viscosity(viscosity) {}
    ~FluidParameters() {}

    // Global simulation parameters

    double damping;

    // particle parameters
    double density;
    double viscosity;
};

struct Fluid {
    Fluid() {}
    Fluid(double width, double height, double depth, int num_width_points,
          int num_height_points, int num_depth_points);
    ~Fluid();

    void buildFluid();

    void simulate(double frames_per_sec, double simulation_steps, FluidParameters *fp,
                  vector<Vector3D> external_accelerations,
                  vector<CollisionObject *> *collision_objects);

    void reset();
    //void buildFluidMesh(); TODO: maybe used later?

    void self_collide(Particle &pm, double simulation_steps);
    float hash_position(Vector3D pos);
    void build_spatial_map();

    // Fluid properties
    double width;
    double height;
    double depth;
    int num_width_points;
    int num_height_points;
    int num_depth_points;
    double thickness;
    e_orientation orientation;

    // Fluid components
    vector<Particle> particles;
    vector<vector<int>> pinned;
    vector<Spring> springs;
    FluidMesh *fluidMesh;

    // Spatial hashing (legacy code and shall be replaced with FLANN)
    unordered_map<float, vector<Particle *> *> map;

    bool indexInRange(int x, int y);

    int flatten(int x, int y);

    void clear_forces();
};

#endif /* FLUID_H */
