//
// Created by 修磊 on 2018/11/30.
//

#ifndef __I_IPARTICL_EFLUID_RESISTANCE_AFFECTOR_H_INCLUDE__
#define __I_IPARTICL_EFLUID_RESISTANCE_AFFECTOR_H_INCLUDE__

#include "IParticleAffector.h"

namespace irr
{
    namespace scene
    {
        class IParticleFluidResistanceAffector : public IParticleAffector
        {
        public:
            IParticleFluidResistanceAffector(float minAcceleration = -0.01f,
                                             float maxAcceleration = -0.005f,
                                             float minDisturbanceX = 0.0f,
                                             float maxDisturbanceX = 0.0f,
                                             float minDisturbanceY = 0.0f,
                                             float maxDisturbanceY = 0.0f,
                                             float minDisturbanceZ = 0.0f,
                                             float maxDisturbanceZ = 0.0f,
                                             u32 minDisturbanceTimeDis = 0,
                                             u32 maxDisturbanceTimeDis = 0) {}

            virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_FLUIDRESISTANCE; }
        };
    }
}

#endif //__I_IPARTICL_EFLUID_RESISTANCE_AFFECTOR_H_INCLUDE__
