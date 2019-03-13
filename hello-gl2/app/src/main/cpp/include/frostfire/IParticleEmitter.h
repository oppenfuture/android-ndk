// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_EMITTER_H_INCLUDED__
#define __I_PARTICLE_EMITTER_H_INCLUDED__

#include "IAttributeExchangingObject.h"
#include "SParticle.h"
#include "irrArray.h"
#include "matrix4.h"

namespace irr
{
namespace scene
{

//! Types of built in particle emitters
enum E_PARTICLE_EMITTER_TYPE
{
	EPET_POINT = 0,
	EPET_ANIMATED_MESH,
	EPET_BOX,
	EPET_CYLINDER,
	EPET_MESH,
	EPET_RING,
	EPET_SPHERE,
	EPET_CONE,
	EPET_COUNT
};

//! Names for built in particle emitters
const c8* const ParticleEmitterTypeNames[] =
{
	"Point",
	"AnimatedMesh",
	"Box",
	"Cylinder",
	"Mesh",
	"Ring",
	"Sphere",
	"Cone",
	0
};

// 发射速度模式
enum EmitteVelocityMode
{
	VELOCITY_AXIS_MODE, // 表示沿着xyz三个轴的方向去产生
	VELOCITY_SHAPE_MODE,	// 表示速度会沿着粒子初始位置与发射器位置连线方向产生
	// （如circle发射器形状下，当velocity的三个分量之和大于0时，会向外扩散，三个分量之和小于0时会向内收拢，
	// 其速度大小为velocity向量的模加上velocity_var的模范围内随机值）
	VELOCITY_CAMERA_MODE	// 粒子速度与相机的朝向一致（比如子弹效果）
};

// 发射加速度模式
enum EmitteAccelerationMode
{
	ACCELERATION_AXIS_MODE,
	ACCELERATION_SHAPE_MODE,
	ACCELERATION_CAMERA_MODE
};

//! A particle emitter for using with particle systems.
/** A Particle emitter emitts new particles into a particle system.
*/
class IParticleEmitter : public virtual io::IAttributeExchangingObject
{
public:
	IParticleEmitter(const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
                     u32 minParticlesPerSecond = 20,
                     u32 maxParticlesPerSecond = 40,
                     const video::SColor& minStartColor = video::SColor(255,0,0,0),
                     const video::SColor& maxStartColor = video::SColor(255,255,255,255),
                     u32 lifeTimeMin=2000,
                     u32 lifeTimeMax=4000,
                     s32 maxAngleDegrees=0,
                     const core::dimension2df& minStartSize = core::dimension2df(5.0f,5.0f),
                     const core::dimension2df& maxStartSize = core::dimension2df(5.0f,5.0f));

	// 设置是否发射
	virtual void SetIsEmitt(bool isEmitt) { IsEmitt = isEmitt; }

	// 获取是否发射
	virtual bool GetIsEmitt() const { return IsEmitt; }

	// 设置粒子公转的每秒最小角度
	virtual void setRevolutionMinAngle(float revolutionMinAngle) { RevolutionMinAngle = revolutionMinAngle; }

	// 获取粒子公转的每秒最小角度
	virtual float getRevolutionMinAngle() const { return RevolutionMinAngle; }

	// 设置粒子公转的每秒最大角度
	virtual void setRevolutionMaxAngle(float revolutionMaxAngle) { RevolutionMaxAngle = revolutionMaxAngle; }

	// 获取粒子公转的每秒最大角度
	virtual float getRevolutionMaxAngle() const { return RevolutionMaxAngle; }

	// 设置粒子公转的旋转轴
	virtual void setRevolutionAxis(const core::vector3df& revolutionAxis) { RevolutionAxis = revolutionAxis; }

	// 获取粒子公转的旋转轴
	virtual const core::vector3df& getRevolutionAxis() const { return RevolutionAxis; }

	// 设置粒子公转的旋转轴范围
	virtual void setRevolutionAxisVar(const core::vector3df& revolutionAxisVar) { RevolutionAxisVar = revolutionAxisVar; }

	// 获取粒子公转的旋转轴范围
	virtual const core::vector3df& getRevolutionAxisVar() const { return RevolutionAxisVar; }

    // 设置初始颜色
    virtual void setStartColor(const video::SColor& startColor) { StartColor = startColor; IsSetStartColor = true; }

    // 获取初始颜色
    virtual const video::SColor& getStartColor() const { return StartColor; }

    // 设置初始颜色范围
    virtual void setStartColorVar(const video::SColor& startColorVar) { StartColorVar = startColorVar; }

    // 获取初始颜色范围
    virtual const video::SColor& getStartColorVar() const { return StartColorVar; }

    // 设置中间颜色
    virtual void setMidColor(const video::SColor& midColor) { MidColor = midColor; IsSetMidColor = true; }

    // 获取中间颜色
    virtual const video::SColor& getMidColor() const { return MidColor; }

    // 设置中间颜色范围
    virtual void setMidColorVar(const video::SColor& midColorVar) { MidColorVar = midColorVar; }

    // 获取中间颜色范围
    virtual const video::SColor& getMidColorVar() const { return MidColorVar; }

    // 设置结束颜色
    virtual void setEndColor(const video::SColor& endColor) { EndColor = endColor; IsSetEndColor = true; }

    // 获取结束颜色
    virtual const video::SColor& getEndColor() const { return EndColor; }

    // 设置结束颜色
    virtual void setEndColorVar(const video::SColor& endColorVar) { EndColorVar = endColorVar; }

    // 获取结束颜色
    virtual const video::SColor& getEndColorVar() const { return EndColorVar; }

	// 设置自转每秒最小角度
	virtual void setSpinAngleMin(float spinAngleMin) { SpinAngleMin = spinAngleMin; }

	// 获取自转每秒最小角度
	virtual float getSpinAngleMin() const  { return SpinAngleMin; }

	// 设置自转每秒最大角度
	virtual void setSpinAngleMax(float spinAngleMax) { SpinAngleMax = spinAngleMax; }

	// 获取自转每秒最大角度
	virtual float getSpinAngleMax() const  { return SpinAngleMax; }

	// 设置相机的位置，用于速度模式
    void setCameraPos(const core::vector3df& cameraPos) { CameraPos = cameraPos; }

    // 获取速度模式
    const core::vector3df& getCameraPos() const { return CameraPos; }

	//! Prepares an array with new particles to emitt into the system
	/** \param now Current time.
	\param timeSinceLastCall Time elapsed since last call, in milliseconds.
	\param outArray Pointer which will point to the array with the new
	particles to add into the system.
	\return Amount of new particles in the array. Can be 0. */
	virtual s32 emitt(u32 now, u32 timeSinceLastCall, SParticle*& outArray);

	// 设置发射初始速度范围
	virtual void setVectorVar(const core::vector3df& vectorVar) { VectorVar = vectorVar; }

	// 获取发射初始速度范围
	virtual const core::vector3df& getVectorVar() const { return VectorVar; }

    // 设置发射加速度
    virtual void setAcceleration(const core::vector3df& acceleration) { Acceleration = acceleration; }

    // 获取发射加速度
    virtual const core::vector3df& getAcceleration() const { return Acceleration; }

	// 设置发射初始加速度范围
	virtual void setAccelerationVar(const core::vector3df& accelerationVar) { AccelerationVar = accelerationVar; }

	// 获取发射初始加速度范围
	virtual const core::vector3df& getAccelerationVar() const { return AccelerationVar; }

	// 设置发射速度模式
	virtual void setEmitteVelocityMode(EmitteVelocityMode velocityMode) { VelocityMode = velocityMode; }

	// 获取发射速度模式
	virtual EmitteVelocityMode getEmitteVelocityMode() const { return VelocityMode; }

	// 设置发射加速度模式
	virtual void setEmitteAccelerationMode(EmitteAccelerationMode accelerationMode) { AccelerationMode = accelerationMode; }

	// 获取发射加速度模式
	virtual EmitteAccelerationMode getEmitteAccelerationMode() const { return AccelerationMode; }

	// 设置发射形状的缩放
	virtual void setEmittShapeScale(const core::vector3df& emittShapeScale) { EmittShapeScale = emittShapeScale; }

	// 获取发射形状的缩放
	virtual const core::vector3df& getEmittShapeScale() const { return EmittShapeScale; }

	// 设置发射器的位置
	virtual void setPosition(const core::vector3df& position) { Position = position; }

	// 获取发射器的位置
	virtual const core::vector3df& getPosition() const { return Position; }

	// 设置发射器旋转的最小角度
	virtual void setMinRotateAngle(float minRotateAngle) { MinRotateAngle = minRotateAngle; }

	// 获取发射器旋转的最小角度
	virtual float getMinRotateAngle() const  { return MinRotateAngle; }

	// 设置发射器旋转的最大角度
	virtual void setMaxRotateAngle(float maxRotateAngle) { MaxRotateAngle = maxRotateAngle; }

	// 获取发射器旋转的最大角度
	virtual float getMaxRotateAngle() const  { return MaxRotateAngle; }

	// 设置发射器的旋转轴
	virtual void setRotateAxis(const core::vector3df& rotateAxis) { RotateAxis = rotateAxis; }

	// 获取发射器的旋转轴
	virtual const core::vector3df& getRotateAxis() const  { return RotateAxis; }

	// 设置发射器旋转轴的范围
	virtual void setRotateAxisVar(const core::vector3df& rotateAxisVar) { RotateAxisVar = rotateAxisVar; }

	// 获取发射器旋转轴的范围
	virtual const core::vector3df& getRotateAxisVar() const  { return RotateAxisVar; }

	// 设置发射的延迟时间
	virtual void setEmittDelayTime(u32 emittDelayTime) { EmittDelayTime = emittDelayTime; }

	// 获取发射的延迟时间
	virtual u32 getEmittDelayTime() const { return EmittDelayTime; }

	// 设置发射的次数
	virtual void setEmittTimes(u32 emittTimes) { EmittTimes = emittTimes; }

	// 获取发射的次数
	virtual u32 getEmittTimes() const { return EmittTimes; }

	// 设置一次发射的时间
	virtual void setEmittOnceTime(u32 emittOnceTime) { EmittOnceTime = emittOnceTime; }

	// 获取一次发射的时间
	virtual u32 getEmittOnceTime() const { return EmittOnceTime; }

	// 设置发射的冷却时间
	virtual void setEmittCoolingTime(u32 emittCoolingTime) { EmittCoolingTime = emittCoolingTime; }

	// 获取发射的冷却时间
	virtual u32 getEmittCoolingTime() const { return EmittCoolingTime; }

	// 设置发射一次最小的粒子数
	virtual void setEmittOnceMinParticle(u32 emittOnceMinParticle) { EmittOnceMinParticle = emittOnceMinParticle; }

	// 获取发射一次发射最小的粒子
	virtual u32 getEmittOnceMinParticle() const { return EmittOnceMinParticle; }

	// 设置发射一次最大的粒子数
	virtual void setEmittOnceMaxParticle(u32 emittOnceMaxParticle) { EmittOnceMaxParticle = emittOnceMaxParticle; }

	// 获取发射一次最大的粒子数
	virtual u32 getEmittOnceMaxParticle() const { return EmittOnceMaxParticle; }

	// 设置最大的粒子个数，不是一次的，是所有的
	virtual void setMaxParticleCount(int maxParticleCount) { MaxParticleCount =  maxParticleCount; }

	// 获取最大的粒子个数
	virtual int getMaxParticleCount() const { return MaxParticleCount; }

	// 设置已经存在的粒子个数，和最大粒子数一起使用，确定要产生粒子的个数
	virtual void setExitParticleCount(int exitParticleCount) { ExitPartilceCount = exitParticleCount; }

	// 获取已经存在的粒子数
	virtual int getExitParticleCount() const { return ExitPartilceCount; }

    //! Set direction the emitter emits particles.
    virtual void setDirection( const core::vector3df& newDirection ) { Direction = newDirection; }

    //! Set minimum number of particles emitted per second.
    virtual void setMinParticlesPerSecond( u32 minPPS ) { MinParticlesPerSecond = minPPS; }

    //! Set maximum number of particles emitted per second.
    virtual void setMaxParticlesPerSecond( u32 maxPPS ) { MaxParticlesPerSecond = maxPPS; }

    //! Set minimum start color.
    virtual void setMinStartColor( const video::SColor& color ) { MinStartColor = color; }

    //! Set maximum start color.
    virtual void setMaxStartColor( const video::SColor& color ) { MaxStartColor = color; }

    //! Set the maximum starting size for particles
    virtual void setMaxStartSize( const core::dimension2df& size ) { MaxStartSize = size; }

    //! Set the minimum starting size for particles
    virtual void setMinStartSize( const core::dimension2df& size ) { MinStartSize = size; }

    // 设置最小的结束大小
    virtual void setMinEndSize( const core::dimension2df& size ) { MinEndSize = size; }

    // 设置最大的结束大小
    virtual void setMaxEndSize( const core::dimension2df& size ) { MaxEndSize = size; }

    //! Set the minimum particle life-time in milliseconds
    virtual void setMinLifeTime( u32 lifeTimeMin ) { MinLifeTime = lifeTimeMin; }

    //! Set the maximum particle life-time in milliseconds
    virtual void setMaxLifeTime( u32 lifeTimeMax ) { MaxLifeTime = lifeTimeMax; }

    //!	Maximal random derivation from the direction
    virtual void setMaxAngleDegrees( s32 maxAngleDegrees ) { MaxAngleDegrees = maxAngleDegrees; }

    //! Gets direction the emitter emits particles.
    virtual const core::vector3df& getDirection() const { return Direction; }

    //! Gets minimum number of particles emitted per second.
    virtual u32 getMinParticlesPerSecond() const { return MinParticlesPerSecond; }

    //! Gets maximum number of particles emitted per second.
    virtual u32 getMaxParticlesPerSecond() const { return MaxParticlesPerSecond; }

    //! Gets minimum start color.
    virtual const video::SColor& getMinStartColor() const { return MinStartColor; }

    //! Gets maximum start color.
    virtual const video::SColor& getMaxStartColor() const { return MaxStartColor; }

    //! Gets the maximum starting size for particles
    virtual const core::dimension2df& getMaxStartSize() const { return MaxStartSize; }

    //! Gets the minimum starting size for particles
    virtual const core::dimension2df& getMinStartSize() const { return MinStartSize; }

    // 获取最小的结束大小
    virtual const core::dimension2df& getMinEndSize() const { return MinEndSize; }

    // 获取最大的结束大小
    virtual const core::dimension2df& getMaxEndSize() const { return MaxEndSize; }

    //! Get the minimum particle life-time in milliseconds
    virtual u32 getMinLifeTime() const { return MinLifeTime; }

    //! Get the maximum particle life-time in milliseconds
    virtual u32 getMaxLifeTime() const { return MaxLifeTime; }

    //!	Maximal random derivation from the direction
    virtual s32 getMaxAngleDegrees() const { return MaxAngleDegrees; }

	//! Get emitter type
	virtual E_PARTICLE_EMITTER_TYPE getType() const { return EPET_POINT; }

	// 获取中心，为计算粒子与当前中心距离使用
	virtual core::vector3df GetCenter() { return core::vector3df(0.0f, 0.0f, 0.0f); }

protected:
    // 设置粒子的初始位置
	virtual void setParticlePosition(core::array<SParticle>& particles) {}

    // 设置粒子的初始速度
    virtual void setParticleVector(core::array<SParticle>& particles);

    // 设置粒子的速度方向沿着轴向
    virtual void setParticleAxisVector(core::array<SParticle>& particles);

    // 设置粒子的速度方向沿着粒子与发射器连线
    virtual void setParticleShapeVector(core::array<SParticle>& particles) {}

    // 设置粒子的速度方向与相机方向相同
    virtual void setParticleCameraVector(core::array<SParticle>& particles);

    // 设置粒子的初始加速度
    virtual void setParticleAcceleration(core::array<SParticle>& particles);

    // 设置粒子的加速度方向沿着轴向
    virtual void setParticleAxisAcceleration(core::array<SParticle>& particles);

    // 设置粒子的加速度方向沿着粒子与发射器连线
    virtual void setParticleShapeAcceleration(core::array<SParticle>& particles) {}

    // 设置粒子的加速度方向与相机方向相同
    virtual void setParticleCameraAcceleration(core::array<SParticle>& particles);

protected:
	bool IsEmitt;
    core::vector3df CameraPos;
    core::vector3df Acceleration;
	core::vector3df VectorVar;
	core::vector3df AccelerationVar;
	EmitteVelocityMode VelocityMode;
	EmitteAccelerationMode AccelerationMode;
	core::vector3df EmittShapeScale;
	core::vector3df Position;
	float MinRotateAngle;
	float MaxRotateAngle;
	float RotateAngle;
	core::vector3df RotateAxis;
	core::vector3df RotateAxisVar;

	bool FirstTimeEmitte;

	u32 EmittDelayTime;

	u32 EmittTimes;

	u32 EmittOnceTime;

	u32 EmittCoolingTime;

	u32 EmittOnceMinParticle;
	u32 EmittOnceMaxParticle;

	u32 AddDelayTime;
	u32 EmittCount;
	u32 AddEmittTime;
	u32 EmittOnceParticle;
	u32 AddEmittOnceParticle;
	u32 AddCoolingTime;

	core::matrix4 EmitterTransfrom;

	///
	core::array<SParticle> Particles;
	core::vector3df Direction;
	core::dimension2df MinStartSize;
	core::dimension2df MaxStartSize;
    core::dimension2df MinEndSize;
    core::dimension2df MaxEndSize;
	u32 MinParticlesPerSecond;
	u32 MaxParticlesPerSecond;
	video::SColor MinStartColor;
	video::SColor MaxStartColor;
	u32 MinLifeTime;
	u32 MaxLifeTime;
	u32 Time;
	s32 MaxAngleDegrees;

	int MaxParticleCount;
	int ExitPartilceCount;

    video::SColor StartColor;
	bool IsSetStartColor;
    video::SColor StartColorVar;
    video::SColor MidColor;
	bool IsSetMidColor;
    video::SColor MidColorVar;
    video::SColor EndColor;
	bool IsSetEndColor;
    video::SColor EndColorVar;
	float SpinAngleMin;
	float SpinAngleMax;
	float RevolutionMinAngle;
	float RevolutionMaxAngle;
	core::vector3df RevolutionAxis;
	core::vector3df RevolutionAxisVar;
};

typedef IParticleEmitter IParticlePointEmitter;

} // end namespace scene
} // end namespace irr


#endif

