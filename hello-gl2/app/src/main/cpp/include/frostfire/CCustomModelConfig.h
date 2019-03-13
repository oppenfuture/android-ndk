// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_CUSTOM_MODELCONFIG_H_INCLUDED__
#define __C_CUSTOM_MODELCONFIG_H_INCLUDED__
#include "IVideoDriver.h"
#include "SMaterial.h"


namespace irr
{
namespace video
{

   	//! Struct for holding parameters for a custom model config
	class CCustomModelConfig
	{
	public:
		
        CCustomModelConfig(const char* archivePath,io::IReadFile* file,video::IVideoDriver* driver);
        ~CCustomModelConfig();
		core::stringw Version;
        core::stringw Model;
        f32 Scale;
        core::vector3df Position;
		f32 NearDistance;
		f32 FarDistance;
        core::vector3df Rotation;
		bool IsShowshadow;
		s32 BackgroundType;
		s32 RotateType;
        core::array<SMaterial> SMaterials;

    protected:
        void Parse(Json::Value& value);
		void CreateSmaterial(Json::Value& value);

		void GetVectorValues(const Json::Value &jsValue, core::array<float> &uniformValues);
		const char* ArchivePath;
        video::IVideoDriver* Driver;
		void ParseVector3df(core::vector3df& vector, Json::Value& value);
    };
    
} // end namespace video
} // end namespace irr

#endif
