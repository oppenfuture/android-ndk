// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_SPHERE_EMITTER_H_INCLUDED__
#define __I_PARTICLE_SPHERE_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"

namespace irr
{
namespace scene
{

//! A particle emitter which emits from a spherical space.
class IParticleSphereEmitter : public IParticleEmitter
{
public:
	IParticleSphereEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
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
							 minStartSize, maxStartSize)
	{}

	//! Set the center of the sphere for particle emissions
	virtual void setCenter( const core::vector3df& center ) = 0;

	//! Set the radius of the sphere for particle emissions
	virtual void setRadius( f32 radius ) = 0;

	//! Get the center of the sphere for particle emissions
	virtual const core::vector3df& getCenter() const = 0;

	//! Get the radius of the sphere for particle emissions
	virtual f32 getRadius() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_SPHERE; }
};

} // end namespace scene
} // end namespace irr


#endif

