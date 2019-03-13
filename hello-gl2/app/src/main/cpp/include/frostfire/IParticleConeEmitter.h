//
// Created by 修磊 on 2018/12/14.
//

#ifndef __I_PARTICLE_CONE_EMITTER_H_INCLUDED__
#define __I_PARTICLE_CONE_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"

namespace irr
{
    namespace scene
    {
        class IParticleConeEmitter : public IParticleEmitter
        {
        public:
            IParticleConeEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
                                 u32 minParticlesPerSecond = 20,
                                 u32 maxParticlesPerSecond = 40,
                                 const video::SColor& minStartColor = video::SColor(255,0,0,0),
                                 const video::SColor& maxStartColor = video::SColor(255,255,255,255),
                                 u32 lifeTimeMin=2000,
                                 u32 lifeTimeMax=4000,
                                 s32 maxAngleDegrees=0,
                                 const core::dimension2df& minStartSize = core::dimension2df(5.0f,5.0f),
                                 const core::dimension2df& maxStartSize = core::dimension2df(5.0f,5.0f)) :
            IParticleEmitter(direction, minParticlesPerSecond, maxParticlesPerSecond,
                    minStartColor, maxStartColor,
                    lifeTimeMin, lifeTimeMax, maxAngleDegrees,
                    minStartSize, maxStartSize) {}

            virtual void setCenter(const core::vector3df& center) = 0;

            virtual const core::vector3df& getCenter() const = 0;

            virtual void setNormal(const core::vector3df& normal) = 0;

            virtual const core::vector3df& getNormal() const = 0;

            virtual void setRadius(float radius) = 0;

            virtual float getRadius() const = 0;

            virtual void setLength(float length) = 0;

            virtual float getLength() const = 0;

            virtual void setAngle(float angle) = 0;

            virtual float getAngle() const = 0;

            virtual void setIsOutlineOnly(bool isOutlineOnly) = 0;

            virtual bool getIsOutlineOnly() const = 0;

            virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_CONE; }
        };
    }
}


#endif //__I_PARTICLE_CONE_EMITTER_H_INCLUDED__
