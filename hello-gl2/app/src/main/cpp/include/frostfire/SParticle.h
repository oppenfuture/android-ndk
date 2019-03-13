// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_PARTICLE_H_INCLUDED__
#define __S_PARTICLE_H_INCLUDED__

#include "vector3d.h"
#include "vector2d.h"
#include "dimension2d.h"
#include "SColor.h"

namespace irr
{
namespace scene
{
	//! Struct for holding particle data
	struct SParticle
	{
		SParticle() : spinAngle(0.0f), revolutionAngle(0.0f), rotate(0.0f),
					  startColorVar(video::SColor(0, 0, 0, 0)),
					  midColorVar(video::SColor(0, 0, 0, 0)),
					  endColorVar(video::SColor(0, 0, 0, 0)),
					  strechScale(core::vector2df(1.0f, 1.0f))
		{}

		//! Position of the particle
		core::vector3df pos;

		float rotate;

        // 公转角度
        float revolution;

		//! Start life time of the particle
		u32 startTime;

		//! End life time of the particle
		u32 endTime;

		//! Current color of the particle
		video::SColor color;

		// 起始颜色范围
		video::SColor startColorVar;

		//! Original color of the particle.
		/** That's the color of the particle it had when it was emitted. */
		video::SColor startColor;

		// 中期颜色范围
		video::SColor midColorVar;

		// 中期颜色
		video::SColor midColor;

		// 结束颜色范围
		video::SColor endColorVar;

		// 结束颜色
		video::SColor endColor;

		//! Direction and speed of the particle
		core::vector3df vector;

		// 初始速度范围
        core::vector3df vectorVar;

		//! Original direction and speed of the particle.
		/** The direction and speed the particle had when it was emitted. */
		core::vector3df startVector;

        // 加速度
        core::vector3df acceleration;

        // 初始加速度范围
        core::vector3df accelerationVar;

        // 初始加速度
        core::vector3df startAcceleration;

		//! Scale of the particle.
		/** The current scale of the particle. */
		core::dimension2df size;

		//! Original scale of the particle.
		/** The scale of the particle when it was emitted. */
		core::dimension2df startSize;

        // 结束大小
        core::dimension2df endSize;

		// 拉伸缩放
		core::vector2df strechScale;

		// 粒子自转每秒的角度
		float spinAngle;

		// 粒子公转每秒的角度
		float revolutionAngle;

		// 粒子公转的旋转轴
		core::vector3df revolutionAxis;

        // 粒子的ID，用于识别某个粒子
        u32 ParticleID;
	};


} // end namespace scene
} // end namespace irr

#endif

