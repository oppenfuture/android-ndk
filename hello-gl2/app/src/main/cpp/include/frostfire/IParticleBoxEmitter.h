// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_BOX_EMITTER_H_INCLUDED__
#define __I_PARTICLE_BOX_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"
#include "aabbox3d.h"

namespace irr
{
namespace scene
{

//! A particle emitter which emits particles from a box shaped space
class IParticleBoxEmitter : public IParticleEmitter
{
public:
	IParticleBoxEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
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

	//! Set the box shape
	virtual void setBox( const core::aabbox3df& box ) = 0;

	//! Get the box shape set
	virtual const core::aabbox3df& getBox() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_BOX; }
};

} // end namespace scene
} // end namespace irr


#endif

