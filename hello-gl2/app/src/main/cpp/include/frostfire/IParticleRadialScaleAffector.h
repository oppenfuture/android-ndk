//
// Created by 修磊 on 2018/12/3.
//

#ifndef __I_PARTICLE_RADIAL_SCALE_AFFECTOR_H_INCLUDE__
#define __I_PARTICLE_RADIAL_SCALE_AFFECTOR_H_INCLUDE__

#include "IParticleAffector.h"

namespace irr
{
    namespace scene
    {
        class IParticleRadialScaleAffector : public IParticleAffector
        {
        public:
            IParticleRadialScaleAffector(float limiteMaxScaleX = 20.0f,
                                         float limiteMaxScaleY = 20.0f,
                                         float minScaleX = 15.0f,
                                         float maxScaleX = 20.0f,
                                         float minScaleY = 0.05f,
                                         float maxScaleY = 0.1f,
                                         ParticleScaleType scaleType = PARTICLE_KEEP_SCALE,
                                         ParticleScaleSpeedType scaleSpeedType = PARTICLE_SCALE_DISTANCE_SPEED,
                                         const core::vector3df& distanceOrigin = core::vector3df(0.0f, 0.0f, 0.0f),
                                         float speedRate = 0.1f) {}

            virtual void affect(u32 now, SParticle* particlearray, u32 count) _IRR_OVERRIDE_ = 0;

            virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_RADIAL_SCALE; }
        };
    }
}

#endif //__I_PARTICLE_RADIAL_SCALE_AFFECTOR_H_INCLUDE__
