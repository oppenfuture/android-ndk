// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_H_INCLUDED__
#define __S_MATERIAL_H_INCLUDED__

#include "SColor.h"
#include "matrix4.h"
#include "irrArray.h"
#include "irrMath.h"
#include "EMaterialTypes.h"
#include "EMaterialFlags.h"
#include "SMaterialLayer.h"
#include "json.h"

#include "ITexture.h"

#if defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#include <OpenGLES/ES2/gl.h>
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#include <GLES2/gl2.h>
#else
#endif

namespace irr
{
    namespace video
    {
        //! Flag for EMT_ONETEXTURE_BLEND, ( BlendFactor ) BlendFunc = source * sourceFactor + dest * destFactor
        enum E_BLEND_FACTOR
        {
            EBF_ZERO	= 0,		//!< src & dest	(0, 0, 0, 0)
            EBF_ONE,			//!< src & dest	(1, 1, 1, 1)
            EBF_DST_COLOR,			//!< src	(destR, destG, destB, destA)
            EBF_ONE_MINUS_DST_COLOR,	//!< src	(1-destR, 1-destG, 1-destB, 1-destA)
            EBF_SRC_COLOR,			//!< dest	(srcR, srcG, srcB, srcA)
            EBF_ONE_MINUS_SRC_COLOR,	//!< dest	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
            EBF_SRC_ALPHA,			//!< src & dest	(srcA, srcA, srcA, srcA)
            EBF_ONE_MINUS_SRC_ALPHA,	//!< src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
            EBF_DST_ALPHA,			//!< src & dest	(destA, destA, destA, destA)
            EBF_ONE_MINUS_DST_ALPHA,	//!< src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
            EBF_SRC_ALPHA_SATURATE		//!< src	(min(srcA, 1-destA), idem, ...)
        };

        //! Values defining the blend operation
        enum E_BLEND_OPERATION
        {
            EBO_NONE = 0,	//!< No blending happens
            EBO_ADD,	//!< Default blending adds the color values
            EBO_SUBTRACT,	//!< This mode subtracts the color values
            EBO_REVSUBTRACT,//!< This modes subtracts destination from source
            EBO_MIN,	//!< Choose minimum value of each color channel
            EBO_MAX,	//!< Choose maximum value of each color channel
            EBO_MIN_FACTOR,	//!< Choose minimum value of each color channel after applying blend factors, not widely supported
            EBO_MAX_FACTOR,	//!< Choose maximum value of each color channel after applying blend factors, not widely supported
            EBO_MIN_ALPHA,	//!< Choose minimum value of each color channel based on alpha value, not widely supported
            EBO_MAX_ALPHA	//!< Choose maximum value of each color channel based on alpha value, not widely supported
        };

        //! MaterialTypeParam: e.g. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
        enum E_MODULATE_FUNC
        {
            EMFN_MODULATE_1X	= 1,
            EMFN_MODULATE_2X	= 2,
            EMFN_MODULATE_4X	= 4
        };

        //! Comparison function, e.g. for depth buffer test
        enum E_COMPARISON_FUNC
        {
            //! Depth test disabled (disable also write to depth buffer)
                    ECFN_DISABLED=0,
            //! <= test, default for e.g. depth test
                    ECFN_LESSEQUAL=1,
            //! Exact equality
                    ECFN_EQUAL=2,
            //! exclusive less comparison, i.e. <
                    ECFN_LESS,
            //! Succeeds almost always, except for exact equality
                    ECFN_NOTEQUAL,
            //! >= test
                    ECFN_GREATEREQUAL,
            //! inverse of <=
                    ECFN_GREATER,
            //! test succeeds always
                    ECFN_ALWAYS,
            //! Test never succeeds
                    ECFN_NEVER
        };


        //! Source of the alpha value to take
        /** This is currently only supported in EMT_ONETEXTURE_BLEND. You can use an
        or'ed combination of values. Alpha values are modulated (multiplicated). */
        enum E_ALPHA_SOURCE
        {
            //! Use no alpha, somewhat redundant with other settings
                    EAS_NONE=0,
            //! Use vertex color alpha
                    EAS_VERTEX_COLOR,
            //! Use texture alpha channel
                    EAS_TEXTURE
        };

        //! Pack srcFact, dstFact, Modulate and alpha source to MaterialTypeParam or BlendFactor
        /** alpha source can be an OR'ed combination of E_ALPHA_SOURCE values. */
        inline f32 pack_textureBlendFunc(const E_BLEND_FACTOR srcFact, const E_BLEND_FACTOR dstFact,
                                         const E_MODULATE_FUNC modulate=EMFN_MODULATE_1X, const u32 alphaSource=EAS_TEXTURE)
        {
            const u32 tmp = (alphaSource << 20) | (modulate << 16) | (srcFact << 12) | (dstFact << 8) | (srcFact << 4) | dstFact;
            return FR(tmp);
        }

        //! Pack srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, Modulate and alpha source to MaterialTypeParam or BlendFactor
        /** alpha source can be an OR'ed combination of E_ALPHA_SOURCE values. */
        inline f32 pack_textureBlendFuncSeparate(const E_BLEND_FACTOR srcRGBFact, const E_BLEND_FACTOR dstRGBFact,
                                                 const E_BLEND_FACTOR srcAlphaFact, const E_BLEND_FACTOR dstAlphaFact,
                                                 const E_MODULATE_FUNC modulate=EMFN_MODULATE_1X, const u32 alphaSource=EAS_TEXTURE)
        {
            const u32 tmp = (alphaSource << 20) | (modulate << 16) | (srcAlphaFact << 12) | (dstAlphaFact << 8) | (srcRGBFact << 4) | dstRGBFact;
            return FR(tmp);
        }

        //! Unpack srcFact, dstFact, modulo and alphaSource factors
        /** The fields don't use the full byte range, so we could pack even more... */
        inline void unpack_textureBlendFunc(E_BLEND_FACTOR &srcFact, E_BLEND_FACTOR &dstFact,
                                            E_MODULATE_FUNC &modulo, u32& alphaSource, const f32 param)
        {
            const u32 state = IR(param);
            alphaSource = (state & 0x00F00000) >> 20;
            modulo = E_MODULATE_FUNC( ( state & 0x000F0000 ) >> 16 );
            srcFact = E_BLEND_FACTOR ( ( state & 0x000000F0 ) >> 4 );
            dstFact = E_BLEND_FACTOR ( ( state & 0x0000000F ) );
        }

        //! Unpack srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo and alphaSource factors
        /** The fields don't use the full byte range, so we could pack even more... */
        inline void unpack_textureBlendFuncSeparate(E_BLEND_FACTOR &srcRGBFact, E_BLEND_FACTOR &dstRGBFact,
                                                    E_BLEND_FACTOR &srcAlphaFact, E_BLEND_FACTOR &dstAlphaFact,
                                                    E_MODULATE_FUNC &modulo, u32& alphaSource, const f32 param)
        {
            const u32 state = IR(param);
            alphaSource = (state & 0x00F00000) >> 20;
            modulo = E_MODULATE_FUNC( ( state & 0x000F0000 ) >> 16 );
            srcAlphaFact = E_BLEND_FACTOR ( ( state & 0x0000F000 ) >> 12 );
            dstAlphaFact = E_BLEND_FACTOR ( ( state & 0x00000F00 ) >> 8 );
            srcRGBFact = E_BLEND_FACTOR ( ( state & 0x000000F0 ) >> 4 );
            dstRGBFact = E_BLEND_FACTOR ( ( state & 0x0000000F ) );
        }

        //! has blend factor alphablending
        inline bool textureBlendFunc_hasAlpha ( const E_BLEND_FACTOR factor )
        {
            switch ( factor )
            {
                case EBF_SRC_ALPHA:
                case EBF_ONE_MINUS_SRC_ALPHA:
                case EBF_DST_ALPHA:
                case EBF_ONE_MINUS_DST_ALPHA:
                case EBF_SRC_ALPHA_SATURATE:
                    return true;
                default:
                    return false;
            }
        }


        //! These flags are used to specify the anti-aliasing and smoothing modes
        /** Techniques supported are multisampling, geometry smoothing, and alpha
        to coverage.
        Some drivers don't support a per-material setting of the anti-aliasing
        modes. In those cases, FSAA/multisampling is defined by the device mode
        chosen upon creation via irr::SIrrCreationParameters.
        */
        enum E_ANTI_ALIASING_MODE
        {
            //! Use to turn off anti-aliasing for this material
                    EAAM_OFF=0,
            //! Default anti-aliasing mode
                    EAAM_SIMPLE=1,
            //! High-quality anti-aliasing, not always supported, automatically enables SIMPLE mode
                    EAAM_QUALITY=3,
            //! Line smoothing
            //! Careful, enabling this can lead to software emulation under OpenGL
                    EAAM_LINE_SMOOTH=4,
            //! point smoothing, often in software and slow, only with OpenGL
                    EAAM_POINT_SMOOTH=8,
            //! All typical anti-alias and smooth modes
                    EAAM_FULL_BASIC=15,
            //! Enhanced anti-aliasing for transparent materials
            /** Usually used with EMT_TRANSPARENT_ALPHA_REF and multisampling. */
                    EAAM_ALPHA_TO_COVERAGE=16
        };


        //! Flags for the definition of the polygon offset feature
        /** These flags define whether the offset should be into the screen, or towards the eye. */
        enum E_POLYGON_OFFSET
        {
            EPO_BACK=0,
            EPO_FRONT=1
        };

        //! Names for polygon offset direction
        const c8* const PolygonOffsetDirectionNames[] =
                {
                        "Back",
                        "Front",
                        0
                };


        //! Maximum number of texture an SMaterial can have.
        const u32 MATERIAL_MAX_TEXTURES = _IRR_MATERIAL_MAX_TEXTURES_;
        struct SUniformValue
        {
            SUniformValue() : Name(""),UsageTarget(""){}
            core::stringc 	 Name;
            core::stringc 	 UsageTarget;
            core::array<f32> Values;
        };
        class SMaterialPass
        {
        public:
            //! Default constructor. Creates a solid, lit material with white colors
            SMaterialPass()
                    : Thickness(1.0f), ZBuffer(ECFN_LESSEQUAL),
                      ColorMask{true, true, true, true}, BlendOperation(EBO_NONE),
                      BlendSrcFactor(EBF_SRC_ALPHA),BlendDstFactor(EBF_ONE_MINUS_SRC_ALPHA),
                      Wireframe(false), Lighting(true), ZWriteEnable(true), BackfaceCulling(true),
                      FrontfaceCulling(false), FogEnable(false),UseMipMaps(true),
                      Name(0),Mirror(false), CurrentFrameNo(0.0), ProgramID(0)
            { }

            //! Copy constructors
            /** \param other Material to copy from. */
            SMaterialPass(const SMaterialPass& other)
            {
                *this = other;
            }

            //! Assignment operator
            /** \param other Material to copy from. */
            SMaterialPass& operator=(const SMaterialPass& other)
            {
                // Check for self-assignment!
                if (this == &other)
                    return *this;
                Thickness = other.Thickness;
                for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
                {
                    TextureLayer[i] = other.TextureLayer[i];
                }
                Wireframe = other.Wireframe;
                Lighting = other.Lighting;
                ZWriteEnable = other.ZWriteEnable;
                BackfaceCulling = other.BackfaceCulling;
                FrontfaceCulling = other.FrontfaceCulling;
                FogEnable = other.FogEnable;
                ZBuffer = other.ZBuffer;
                for (int i = 0; i < 4; ++i) {
                    ColorMask[i] = other.ColorMask[i];
                }
                BlendOperation = other.BlendOperation;
                BlendSrcFactor = other.BlendSrcFactor;
                BlendDstFactor = other.BlendDstFactor;
                UseMipMaps = other.UseMipMaps;
                Name = other.Name;
                Mirror = other.Mirror;
                CurrentFrameNo = other.CurrentFrameNo;
                ProgramID = other.ProgramID;
                UniformValue.clear();
                for (int i = 0; i < other.UniformValue.size(); ++i) {
                    SUniformValue uniform = SUniformValue();
                    uniform.Name = other.UniformValue[i].Name;
                    uniform.UsageTarget = other.UniformValue[i].UsageTarget;
                    uniform.Values = other.UniformValue[i].Values;
                    UniformValue.push_back(uniform);
                }
                return *this;
            }

            //! Texture layer array.
            SMaterialLayer TextureLayer[MATERIAL_MAX_TEXTURES];

            //! Thickness of non-3dimensional elements such as lines and points.
            f32 Thickness;

            //! Is the ZBuffer enabled? Default: ECFN_LESSEQUAL
            /** If you want to disable depth test for this material
            just set this parameter to ECFN_DISABLED.
            Values are from E_COMPARISON_FUNC. */
            u8 ZBuffer;

            //! Defines the enabled color planes
            bool ColorMask[4]={true,true,true, true};

            //! Store the blend operation of choice
            /** Values to be chosen from E_BLEND_OPERATION. */
            E_BLEND_OPERATION BlendOperation;

            //! Store the blend factors
            /** textureBlendFunc/textureBlendFuncSeparate functions should be used to write
            properly blending factors to this parameter. If you use EMT_ONETEXTURE_BLEND
            type for this material, this field should be equal to MaterialTypeParam. */
            E_BLEND_FACTOR BlendSrcFactor;
            E_BLEND_FACTOR BlendDstFactor;

            //! Draw as wireframe or filled triangles? Default: false
            /** The user can access a material flag using
            \code material.Wireframe=true \endcode
            or \code material.setFlag(EMF_WIREFRAME, true); \endcode */
            bool Wireframe:1;

            //! Will this material be lighted? Default: true
            bool Lighting:1;

            //! Is the zbuffer writeable or is it read-only. Default: true.
            /** This flag is forced to false if the MaterialType is a
            transparent type and the scene parameter
            ALLOW_ZWRITE_ON_TRANSPARENT is not set. If you set this parameter
            to true, make sure that ZBuffer value is other than ECFN_DISABLED */
            bool ZWriteEnable:1;

            //! Is backface culling enabled? Default: true
            bool BackfaceCulling:1;

            //! Is frontface culling enabled? Default: false
            bool FrontfaceCulling:1;

            //! Is fog enabled? Default: false
            bool FogEnable:1;

            //! Shall mipmaps be used if available
            /** Sometimes, disabling mipmap usage can be useful. Default: true */
            bool UseMipMaps:1;

            core::stringw Name;

            bool Mirror;

            f32 CurrentFrameNo;

            core::array<SUniformValue>  UniformValue;

            int ProgramID;

            //! Gets the i-th texture
            /** \param i The desired level.
            \return Texture for texture level i, if defined, else 0. */
            ITexture* getTexture(u32 i) const
            {
                return i < MATERIAL_MAX_TEXTURES ? TextureLayer[i].Texture : 0;
            }

            ITexture* getTexture(core::stringc name){
                ITexture* tex = 0;
                    for (int j = 0; j < UniformValue.size(); ++j) {
                        if (name == UniformValue[j].Name
                            && UniformValue[j].Values.size()> 0
                            && UniformValue[j].Values[0]>=0){
                            int index = UniformValue[j].Values[0];
                            tex = TextureLayer[index].Texture;
                            break;
                        }
                    }
                return tex;
            }
            //! Sets the i-th texture
            /** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
            \param i The desired level.
            \param tex Texture for texture level i. */
            void setTexture(u32 i, ITexture* tex)
            {
                if (i>=MATERIAL_MAX_TEXTURES)
                    return;
                TextureLayer[i].Texture = tex;
                SetTextureUsage(i, true);
            }

            void setColor(core::stringc name,SColorf value){
                SUniformValue* uniform = findUniform(name);
                if(uniform!=NULL){
                    uniform->Values.clear();
                    uniform->Values.push_back(value.r);
                    uniform->Values.push_back(value.g);
                    uniform->Values.push_back(value.b);
                    uniform->Values.push_back(value.a);
                }
            }

            void setVector(core::stringc name,f32* value,int count){
                SUniformValue *uniform = findUniform(name);
                if (uniform != NULL) {
                    uniform->Values.clear();
                    for (int i = 0; i < count; ++i) {
                        uniform->Values.push_back(value[i]);
                    }
                }
            }

            void setFloat(core::stringc name,f32 value){
                SUniformValue *uniform = findUniform(name);
                if (uniform != NULL) {
                    uniform->Values.clear();
                    uniform->Values.push_back(value);
                }
            }

            void setInt(core::stringc name, int value){
                setFloat(name,f32(value));
            }

            void setTexture(core::stringc name,ITexture* value){
                for (int j = 0; j < UniformValue.size(); ++j) {
                    if (name == UniformValue[j].Name
                        && UniformValue[j].Values.size() > 0
                        && UniformValue[j].Values[0] >= 0) {
                        int index = UniformValue[j].Values[0];
                        TextureLayer[index].Texture = value;
                        TextureLayer[index].TrilinearFilter = true;
                        break;
                    }
                }
                for (int j = 0; j < UniformValue.size(); ++j) {
                    if (name == UniformValue[j].UsageTarget) {
                        UniformValue[j].Values.clear();
                        UniformValue[j].Values.push_back(1);
                        break;
                    }
                }
            }
            void updateSubTexture(core::stringc name,u32 xoffset,u32 yoffset,u32 width,u32 height,GLsizei dataSize, GLenum internalformat,void * data ){
                ITexture* tex = 0;
                int index = 0;
                for (int j = 0; j < UniformValue.size(); ++j) {
                    if (name == UniformValue[j].Name
                        && UniformValue[j].Values.size()> 0
                        && UniformValue[j].Values[0]>=0){
                        index = UniformValue[j].Values[0];
                        tex = TextureLayer[index].Texture;
                        break;
                    }
                }
                if(tex)
                    tex->loadRawSubTexture(GL_TEXTURE_2D,xoffset,yoffset,width,height,dataSize,internalformat,data,index);
            }

            void updateTexture(core::stringc name,u32 width,u32 height,GLsizei dataSize, GLenum internalformat,void * data ){
                ITexture* tex = 0;
                int index = 0;
                for (int j = 0; j < UniformValue.size(); ++j) {
                    if (name == UniformValue[j].Name
                        && UniformValue[j].Values.size()> 0
                        && UniformValue[j].Values[0]>=0){
                        index = UniformValue[j].Values[0];
                        tex = TextureLayer[index].Texture;
                        break;
                    }
                }
                if(tex)
                    tex->loadRawTexture(GL_TEXTURE_2D,width,height,dataSize,internalformat,data,index);
            }


            SUniformValue* findUniform(core::stringc name){
                SUniformValue* uniform = 0;
                for (int j = 0; j < UniformValue.size(); ++j) {
                    if (name == UniformValue[j].Name){
                        uniform = &UniformValue[j];
                        break;
                    }
                }

                return uniform;
            }

            //! Inequality operator
            /** \param b Material to compare to.
            \return True if the materials differ, else false. */
            inline bool operator!=(const SMaterialPass& b) const
            {
                bool different = Name != b.Name;
                for (u32 i=0; (i<MATERIAL_MAX_TEXTURES) && !different; ++i)
                {
                    different |= (TextureLayer[i] != b.TextureLayer[i]);
                }
                different |= (BlendDstFactor != b.BlendDstFactor );
                different |= (BlendSrcFactor != b.BlendSrcFactor );
                different |= (BlendOperation != b.BlendOperation );
                different |= (BackfaceCulling != b.BackfaceCulling );
                for (int i = 0; i < UniformValue.size(); ++i) {
                    different |= UniformValue[i].Name != b.UniformValue[i].Name;
                }

                return different;
            }

            //! Equality operator
            /** \param b Material to compare to.
            \return True if the materials are equal, else false. */
            inline bool operator==(const SMaterialPass& b) const
            { return !(b!=*this); }

            bool isTransparent() const
            {
                if (BlendOperation != EBO_NONE )
                {
                    return true;
                }

                return false;
            }

        private:
            void SetTextureUsage(int layer, bool usage){
                for (int i = 0; i < UniformValue.size(); ++i) {
                    if (UniformValue[i].UsageTarget != ""&&UniformValue[i].UsageTarget != NULL ){
                        core::stringc uniformName = UniformValue[i].UsageTarget;
                        for (int j = 0; j < UniformValue.size(); ++j) {
                            if(uniformName == UniformValue[j].Name
                               && UniformValue[j].Values.size()>0
                               && UniformValue[j].Values[0] == layer){
                                UniformValue[i].Values.clear();
                                UniformValue[i].Values.push_back(1);
                                break;
                            }
                        }
                        if (UniformValue[i].Values.size()>0 && UniformValue[i].Values[0]==1)
                            break;
                    }
                }
            }

        };
        //! Struct for holding parameters for a material renderer
        class SMaterial
        {
        public:
            //! Default constructor. Creates a solid, lit material with white colors
            SMaterial()
                    : MaterialType(EMT_SOLID), Wireframe(false), Lighting(true), UseMipMaps(true),
                      Name(0),isEnableOutline(false),
                      RenderName(""),IsShadow(false),IsVisible(true),isGenTanget(false),Queue(2000)
            { }

            //! Copy constructor
            /** \param other Material to copy from. */
            SMaterial(const SMaterial& other)
            {
                *this = other;
            }
            SMaterial(const Json::Value value)
            {
                ParseRenderState(value);
            }
            //! Assignment operator
            /** \param other Material to copy from. */
            SMaterial& operator=(const SMaterial& other)
            {
                // Check for self-assignment!
                if (this == &other)
                    return *this;

                MaterialType = other.MaterialType;
                Wireframe = other.Wireframe;
                Name = other.Name;
                outlineColor = other.outlineColor;
                outlineOffset = other.outlineOffset;
                isEnableOutline = other.isEnableOutline;
                IsShadow = other.IsShadow;
                IsVisible = other.IsVisible;
                RenderName = other.RenderName;
                isGenTanget = other.isGenTanget;
                Queue = other.Queue;
                if( MaterialPassList.size() == 0 )
                {
                    for(int i = 0; i < other.MaterialPassList.size(); i++ )
                    {
                        MaterialPassList.push_back( other.MaterialPassList[i]);
                    }
                }
                else if( MaterialPassList.size() <= other.MaterialPassList.size() )
                {
                    for(int i = 0; i < MaterialPassList.size(); i++ )
                    {
                        MaterialPassList[i] = other.MaterialPassList[i];
                    }

                    for(int i = MaterialPassList.size(); i < other.MaterialPassList.size(); i++ )
                    {
                        MaterialPassList.push_back( other.MaterialPassList[i]);
                    }
                }
                else
                {
                    for(int i = 0; i < other.MaterialPassList.size(); i++ )
                    {
                        MaterialPassList[i] = other.MaterialPassList[i];
                    }

                    for(int i = other.MaterialPassList.size(); i < MaterialPassList.size(); i++ )
                    {
                        MaterialPassList.erase(i);
                    }
                }
                return *this;
            }


            //! Type of the material. Specifies how everything is blended together
            E_MATERIAL_TYPE MaterialType;

            //! Draw as wireframe or filled triangles? Default: false
            bool Wireframe:1;

            //! Will this material be lighted? Default: true
            bool Lighting:1;

            //! Shall mipmaps be used if available
            /** Sometimes, disabling mipmap usage can be useful. Default: true */
            bool UseMipMaps:1;

            core::stringw Name;

            SColorf outlineColor;

            f32 outlineOffset;

            bool Mirror;

            bool IsShadow;

            bool IsVisible;
            int Queue;
            void EnableOutline(const SColorf& outlineColor = SColorf(1.0, 0.0, 0.0), f32 outlineOffset = 0.2)
            {
                isEnableOutline = true;
                this->outlineColor = outlineColor;
                this->outlineOffset = outlineOffset;
            }

            void DisableOutline()
            {
                isEnableOutline = false;
            }

            bool IsEnableOutline() const
            {
                return isEnableOutline;
            }

            //! Gets the i-th texture
            /** \param i The desired level.
            \return Texture for texture level i, if defined, else 0. */
            ITexture* getTexture(int PassID,u32 i) const
            {
                return i < MATERIAL_MAX_TEXTURES ? (getPass(PassID).TextureLayer[i].Texture) : 0;
            }

            //! Sets the i-th texture
            void setTexture(u32 i, ITexture* tex)
            {
                if (i>=MATERIAL_MAX_TEXTURES)
                    return;
                for (int j = 0; j < MaterialPassList.size(); ++j) {
                    getPass(j).setTexture(i,tex);
                }
            }

            //! Inequality operator
            /** \param b Material to compare to.
            \return True if the materials differ, else false. */
            inline bool operator!=(const SMaterial& b) const
            {
                bool diff = Name != b.Name;
                diff |= (RenderName != b.RenderName);
                diff |= MaterialPassList.size() != b.MaterialPassList.size();
                for(u32 i = 0; i < b.MaterialPassList.size() && !diff ; i++)
                {
                    diff |= (MaterialPassList[i] != b.MaterialPassList[i]);
                }
                return diff;
            }

            //! Equality operator
            /** \param b Material to compare to.
            \return True if the materials are equal, else false. */
            inline bool operator==(const SMaterial& b) const
            { return !(b!=*this); }

            bool isTransparent() const
            {
                bool istransparent = false;
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    if(getPass(i).isTransparent()){
                        istransparent = true;
                        break;
                    }
                }
                return istransparent;
            }
            bool isGenTanget;
            core::stringc RenderName;
            mutable core::array<SMaterialPass> MaterialPassList;
            SMaterialPass& getPass(u32 i) const
            {
                if( MaterialPassList.size() == 0 )
                {
                    MaterialPassList.push_back( SMaterialPass() );
                    return MaterialPassList.getLast();
                } else{
                    return MaterialPassList[i];
                }
            }

            void setColor(core::stringc name,SColorf value){
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    SUniformValue* uniform = findUniform( i,name);
                    if(uniform!=NULL){
                        uniform->Values.clear();
                        uniform->Values.push_back(value.r);
                        uniform->Values.push_back(value.g);
                        uniform->Values.push_back(value.b);
                        uniform->Values.push_back(value.a);
                    }
                }
            }

            void setVector(core::stringc name,f32* value,int count){
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    SUniformValue *uniform = findUniform(i,name);
                    if (uniform != NULL) {
                        uniform->Values.clear();
                        for (int i = 0; i < count; ++i) {
                            uniform->Values.push_back(value[i]);
                        }
                    }
                }

            }

            void setFloat(core::stringc name,f32 value){
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    SUniformValue *uniform = findUniform(i,name);
                    if (uniform != NULL) {
                        uniform->Values.clear();
                        uniform->Values.push_back(value);
                    }
                }
            }

            void setInt(core::stringc name, int value){
                setFloat(name,f32(value));
            }
            void setTexture(core::stringc name,ITexture* value){
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name == getPass(i).UniformValue[j].Name
                            && getPass(i).UniformValue[j].Values.size() > 0
                            && getPass(i).UniformValue[j].Values[0] >= 0) {
                            int index = getPass(i).UniformValue[j].Values[0];
                            getPass(i).TextureLayer[index].Texture = value;
                            getPass(i).TextureLayer[index].TrilinearFilter = true;
                            break;
                        }
                    }
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name== getPass(i).UniformValue[j].UsageTarget) {
                            getPass(i).UniformValue[j].Values.clear();
                            getPass(i).UniformValue[j].Values.push_back(1);
                            break;
                        }
                    }
                }
            }
            void setTexture(int passID, core::stringc name,ITexture* value){
                    if (passID>= MaterialPassList.size())
                        passID = MaterialPassList.size()-1;
                    for (int j = 0; j < getPass(passID).UniformValue.size(); ++j) {
                        if (name == getPass(passID).UniformValue[j].Name
                            && getPass(passID).UniformValue[j].Values.size() > 0
                            && getPass(passID).UniformValue[j].Values[0] >= 0) {
                            int index = getPass(passID).UniformValue[j].Values[0];
                            getPass(passID).TextureLayer[index].Texture = value;
                            getPass(passID).TextureLayer[index].TrilinearFilter = true;
                            break;
                        }
                    }
                    for (int j = 0; j < getPass(passID).UniformValue.size(); ++j) {
                        if (name == getPass(passID).UniformValue[j].UsageTarget) {
                            getPass(passID).UniformValue[j].Values.clear();
                            getPass(passID).UniformValue[j].Values.push_back(1);
                            break;
                        }
                    }

            }

            void resetTextureUnit(core::stringc name,int ID){
                if (ID >= MATERIAL_MAX_TEXTURES || ID < 0)
                    return;
                bool isholdingTex = false;
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name == getPass(i).UniformValue[j].Name
                            && getPass(i).UniformValue[j].Values.size() > 0
                            && getPass(i).UniformValue[j].Values[0] >= 0) {
                              getPass(i).UniformValue[j].Values[0] = ID;
                              if (getPass(i).TextureLayer[ID].Texture)
                                  isholdingTex = true;
                            break;
                        }
                    }
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name == getPass(i).UniformValue[j].UsageTarget) {
                            getPass(i).UniformValue[j].Values.clear();
                            getPass(i).UniformValue[j].Values.push_back(int(isholdingTex));
                            break;
                        }
                    }
                }
            }

            SColorf getColor(core::stringc name){
                SColorf color = SColorf(0,0,0,0);
                SUniformValue* uniform = findUniform(name);
                if(uniform!=NULL && uniform->Values.size() == 4){
                    color.r = uniform->Values[0];
                    color.g = uniform->Values[1];
                    color.b = uniform->Values[2];
                    color.a = uniform->Values[3];
                }
                return color;
            }

            f32 getFloat(core::stringc name){
                f32 value = 0;
                SUniformValue* uniform = findUniform(name);
                if(uniform!=NULL && uniform->Values.size()==1) {
                    value = uniform->Values[0];
                }
                return value;
            }

            int getInt(core::stringc name){
                return  (int)getFloat(name);
            }

            ITexture* getTexture(core::stringc name){
                ITexture* tex = 0;
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name == getPass(i).UniformValue[j].Name
                        && getPass(i).UniformValue[j].Values.size()> 0
                        && getPass(i).UniformValue[j].Values[0]>=0){
                            int index = getPass(i).UniformValue[j].Values[0];
                            tex = getPass(i).TextureLayer[index].Texture;
                            break;
                        }
                    }
                }
                return tex;
            }

            SUniformValue* findUniform(int passID,core::stringc name){
                SUniformValue* uniform = 0;
                uniform = getPass(passID).findUniform(name);
                return uniform;
            }

            SUniformValue* findUniform(core::stringc name){
                SUniformValue* uniform = 0;
                for (int i = 0; i < MaterialPassList.size(); ++i) {
                    for (int j = 0; j < getPass(i).UniformValue.size(); ++j) {
                        if (name == getPass(i).UniformValue[j].Name){
                            uniform = &getPass(i).UniformValue[j];
                            break;
                        }
                    }
                }
                return uniform;
            }

            void ParseRenderState(Json::Value RJs) {
                if(RJs.empty()){
                    return;
                }
                RenderName = RJs.get("name","").asCString();
                Queue = RJs.get("queue",2000).asInt();
                Json::Value passArrayJs = RJs.get("pass","");
                for (int j = 0; j <passArrayJs.size() ; ++j) {
                    SMaterialPass pass;
                    Json::Value passJs = passArrayJs.get(j,"");
                    for (int k = 0; k <passJs.size() ; ++k) {
                        if (passJs.get("blendenalble", false).asBool()) {
                            pass.BlendOperation = EBO_ADD;
                        } else {
                            pass.BlendOperation = EBO_NONE;
                        }
                        pass.BlendSrcFactor = GetBlendFacotrValues(passJs.get("blendSrc","SrcAlpha").asCString());
                        pass.BlendDstFactor = GetBlendFacotrValues(passJs.get("blendDst","OneMinusSrcAlpha").asCString());
                        pass.ZWriteEnable = passJs.get("zwrite", true).asBool();
                        pass.ZBuffer = GetZBufferValue(passJs.get("ztest", "LEqual").asCString());

                        core::stringc cullStr = passJs.get("cull","back").asCString();
                        cullStr.make_lower();
                        if (cullStr == "off") {
                            pass.BackfaceCulling = false;
                            pass.FrontfaceCulling = false;
                        } else if (cullStr == "front") {
                            pass.BackfaceCulling = false;
                            pass.FrontfaceCulling = true;
                        } else if (cullStr == "back") {
                            pass.BackfaceCulling = true;
                            pass.FrontfaceCulling = false;
                        }
                        Json::Value colorMaskJs = passJs.get("colormask","");
                        ParseColorMask(pass.ColorMask,colorMaskJs);
                        pass.ProgramID = passJs.get("programid",0).asUInt();
                    }
                    MaterialPassList.push_back(pass);
                }
            }

        private:
            bool isEnableOutline;

            E_BLEND_FACTOR GetBlendFacotrValues(const core::stringc str){
                core:: stringc key = str;
                key = key.make_lower();
                if (key == "one"){
                    return EBF_ONE;
                }else if(key == "zero"){
                    return EBF_ZERO;
                }else if(key == "srcalpha"){
                    return EBF_SRC_ALPHA;
                }else if(key == "dstalpha"){
                    return EBF_DST_ALPHA;
                }else if(key == "srccolor"){
                    return EBF_SRC_COLOR;
                }else if(key == "dstcolor"){
                    return EBF_DST_COLOR;
                }else if(key == "oneminussrcalpha"){
                    return EBF_ONE_MINUS_SRC_ALPHA;
                }else if(key == "oneminusdstalpha"){
                    return EBF_ONE_MINUS_DST_ALPHA;
                }else if(key == "oneminussrccolor"){
                    return EBF_ONE_MINUS_SRC_COLOR;
                }else if(key == "oneminusdstcolor"){
                    return EBF_ONE_MINUS_DST_COLOR;
                } else{
                    return EBF_ONE;
                }
            }
            int GetZBufferValue(const core::stringc str){
                core:: stringc key = str;
                key = key.make_lower();
                if (key == "always"){
                    return ECFN_ALWAYS;
                }else if(key == "equal"){
                    return ECFN_EQUAL;
                }else if(key == "greater"){
                    return ECFN_GREATER;
                }else if(key == "greaterequal"){
                    return ECFN_GREATEREQUAL;
                }else if(key == "less"){
                    return ECFN_LESS;
                }else if(key == "lessequal"){
                    return ECFN_LESSEQUAL;
                }else if(key == "never"){
                    return ECFN_NEVER;
                }else if(key == "notequal"){
                    return ECFN_NOTEQUAL;
                }else if(key == "disabled"){
                    return ECFN_DISABLED;
                } else{
                    return ECFN_EQUAL;
                }
            }
            void ParseColorMask(bool *colorMask,const Json::Value &value){
                core::stringc keys[]={"r","g","b","a"};
                for (int i = 0; i < value.size(); ++i) {
                    colorMask[i] = value.get(keys[i].c_str(),0).asBool();
                }
            }
        };

        //! global const identity Material
        IRRLICHT_API extern SMaterial IdentityMaterial;

    } // end namespace video
} // end namespace irr

#endif
