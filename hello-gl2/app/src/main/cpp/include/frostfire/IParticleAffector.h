// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_AFFECTOR_H_INCLUDED__
#define __I_PARTICLE_AFFECTOR_H_INCLUDED__

#include "IAttributeExchangingObject.h"
#include "SParticle.h"

namespace irr
{
namespace scene
{

//! Types of built in particle affectors
enum E_PARTICLE_AFFECTOR_TYPE
{
	EPAT_NONE = 0,
	EPAT_ATTRACT = 1,
	EPAT_FADE_OUT = 2,
	EPAT_GRAVITY = 4,
	EPAT_ROTATE = 8,
	EPAT_SCALE = 16,
	EPAT_FLUIDRESISTANCE = 32,
	EPAT_PARTICLE_ROTATE = 64,
	EPAT_RADIAL_SCALE = 128,
	EPAT_REVOLUTION = 256,
	EPAT_COUNT = 65536
};

//! Names for built in particle affectors
const c8* const ParticleAffectorTypeNames[] =
{
	"None",
	"Attract",
	"FadeOut",
	"Gravity",
	"Rotate",
	"Scale",
	"FluidResistance",
	"ParticleRotate",
	"RadialScale",
	"Revolution",
	0
};

enum ParticleScaleType
{
	PARTICLE_KEEP_SCALE,
	PARTICLE_ALONG_VECTOR_SCALE,
	PARTICLE_OPPOSITE_ALONG_VECTOR_SCALE
};

enum ParticleScaleSpeedType
{
	PARTICLE_SCALE_VECTOR_SPEED,
	PARTICLE_SCALE_DISTANCE_SPEED,
	PARTICLE_SCALE_CONST_SPEED,
};

//! A particle affector modifies particles.
class IParticleAffector : public virtual io::IAttributeExchangingObject
{
public:

	//! constructor
	IParticleAffector() : Enabled(true) {}

	//! Affects an array of particles.
	/** \param now Current time. (Same as ITimer::getTime() would return)
	\param particlearray Array of particles.
	\param count Amount of particles in array. */
	virtual void affect(u32 now, SParticle* particlearray, u32 count) = 0;

	//! Sets whether or not the affector is currently enabled.
	virtual void setEnabled(bool enabled) { Enabled = enabled; }

	//! Gets whether or not the affector is currently enabled.
	virtual bool getEnabled() const { return Enabled; }

	//! Get emitter type
	virtual E_PARTICLE_AFFECTOR_TYPE getType() const = 0;

protected:
	bool Enabled;
};

} // end namespace scene
} // end namespace irr


#endif

