//
// Created by 修磊 on 2018/12/3.
//

#ifndef __I_PARTICLE_PARTICLE_ROTATE_AFFECTOR_H_INCLUDE__
#define __I_PARTICLE_PARTICLE_ROTATE_AFFECTOR_H_INCLUDE__

#include "IParticleAffector.h"

namespace irr
{
    namespace scene
    {
        class IParticleParticleRotateAffector : public IParticleAffector
        {
        public:
            virtual void affect(u32 now, SParticle* particlearray, u32 count) _IRR_OVERRIDE_ = 0;

            virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_PARTICLE_ROTATE; }
        };
    }
}

#endif //__I_PARTICLE_PARTICLE_ROTATE_AFFECTOR_H_INCLUDE__
