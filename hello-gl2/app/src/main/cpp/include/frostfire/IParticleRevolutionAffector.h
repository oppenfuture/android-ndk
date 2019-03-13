// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_REVOLUTION_AFFECTOR_H_INCLUDED__
#define __I_PARTICLE_REVOLUTION_AFFECTOR_H_INCLUDED__

#include "IParticleAffector.h"

namespace irr
{
namespace scene
{

class IParticleRevolutionAffector : public IParticleAffector
{
public:
	//! Get emitter type
	virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_REVOLUTION; }
};

} // end namespace scene
} // end namespace irr


#endif // __I_PARTICLE_REVOLUTION_AFFECTOR_H_INCLUDED__

