#ifndef _RAM_SIM_PHYSICS_WORLD_H
#define _RAM_SIM_PHYSICS_WORLD_H

namespace ram {
    namespace sim {
        namespace physics {
            class World {
            public:
                World();
                ~World();
                void step(float dt);
                float getGravity() const;
                void setGravity(float);
                float getFluidDensity() const;
                void setFluidDensity(float);
            private:
                float gravity;
                float fluidDensity;
                void stepIntegrate(Body&, float dt);
            };
        }
    }
}

#endif
