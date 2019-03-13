// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_MESH_EMITTER_H_INCLUDED__
#define __I_PARTICLE_MESH_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"
#include "IMesh.h"

namespace irr
{
namespace scene
{

//! A particle emitter which emits from vertices of a mesh
class IParticleMeshEmitter : public IParticleEmitter
{
public:
	IParticleMeshEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
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

	//! Set Mesh to emit particles from
	virtual void setMesh( IMesh* mesh ) = 0;

	//! Set whether to use vertex normal for direction, or direction specified
	virtual void setUseNormalDirection( bool useNormalDirection = true ) = 0;

	//! Set the amount that the normal is divided by for getting a particles direction
	virtual void setNormalDirectionModifier( f32 normalDirectionModifier ) = 0;

	//! Sets whether to emit min<->max particles for every vertex or to pick min<->max vertices
	virtual void setEveryMeshVertex( bool everyMeshVertex = true ) = 0;

	//! Get Mesh we're emitting particles from
	virtual const IMesh* getMesh() const = 0;

	//! Get whether to use vertex normal for direction, or direction specified
	virtual bool isUsingNormalDirection() const = 0;

	//! Get the amount that the normal is divided by for getting a particles direction
	virtual f32 getNormalDirectionModifier() const = 0;

	//! Gets whether to emit min<->max particles for every vertex or to pick min<->max vertices
	virtual bool getEveryMeshVertex() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_MESH; }
};

} // end namespace scene
} // end namespace irr


#endif

