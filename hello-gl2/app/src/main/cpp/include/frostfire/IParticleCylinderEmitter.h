// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_CYLINDER_EMITTER_H_INCLUDED__
#define __I_PARTICLE_CYLINDER_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"

namespace irr
{
namespace scene
{

//! A particle emitter which emits from a cylindrically shaped space.
class IParticleCylinderEmitter : public IParticleEmitter
{
public:
	IParticleCylinderEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
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

	//! Set the center of the radius for the cylinder, at one end of the cylinder
	virtual void setCenter( const core::vector3df& center ) = 0;

	//! Set the normal of the cylinder
	virtual void setNormal( const core::vector3df& normal ) = 0;

	//! Set the radius of the cylinder
	virtual void setRadius( f32 radius ) = 0;

	//! Set the length of the cylinder
	virtual void setLength( f32 length ) = 0;

	//! Set whether or not to draw points inside the cylinder
	virtual void setOutlineOnly( bool outlineOnly = true ) = 0;

	//! Get the center of the cylinder
	virtual const core::vector3df& getCenter() const = 0;

	//! Get the normal of the cylinder
	virtual const core::vector3df& getNormal() const = 0;

	//! Get the radius of the cylinder
	virtual f32 getRadius() const = 0;

	//! Get the center of the cylinder
	virtual f32 getLength() const = 0;

	//! Get whether or not to draw points inside the cylinder
	virtual bool getOutlineOnly() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_CYLINDER; }
};

} // end namespace scene
} // end namespace irr


#endif

