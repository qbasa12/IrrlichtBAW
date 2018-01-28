// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_H_INCLUDED__
#define __S_MATERIAL_H_INCLUDED__

#include "SColor.h"
#include "irrMath.h"
#include "vectorSIMD.h"
#include "EMaterialTypes.h"
#include "EMaterialFlags.h"
#include "SMaterialLayer.h"
#include "IGPUBuffer.h"

namespace irr
{
namespace video
{
	class IVirtualTexture;


	//!
	enum E_POLYGON_RASTER_MODE
	{
	    EPRM_POINT,
	    EPRM_LINE,
	    EPRM_FILL
	};

	//! Comparison function, e.g. for depth and stencil buffer test
	enum E_COMPARISON_FUNC
	{
		//! Test never succeeds, this equals disable
		ECFN_NEVER=0,
		//! <= test
		ECFN_LESSEQUAL=1,
		//! Exact equality
		ECFN_EQUAL=2,
		//! exclusive less comparison, i.e. <
		ECFN_LESS,
		//! Succeeds almost always, except for exact equality
		ECFN_NOTEQUAL,
		//! >= test, default for e.g. depth test
		ECFN_GREATEREQUAL,
		//! inverse of <=
		ECFN_GREATER,
		//! test succeeds always
		ECFN_ALWAYS
	};

	enum E_STENCIL_FUNC
	{
	    ESF_KEEP=0,
	    ESF_ZERO,
	    ESF_REPLACE,
	    ESF_INCR,
	    ESF_INCR_WRAP,
	    ESF_DECR,
	    ESF_DECR_WRAP,
	    ESF_INVERT
	};

	//! Enum values for enabling/disabling color planes for rendering
	enum E_COLOR_PLANE
	{
		//! No color enabled
		ECP_NONE=0,
		//! Alpha enabled
		ECP_ALPHA=1,
		//! Red enabled
		ECP_RED=2,
		//! Green enabled
		ECP_GREEN=4,
		//! Blue enabled
		ECP_BLUE=8,
		//! All colors, no alpha
		ECP_RGB=14,
		//! All planes enabled
		ECP_ALL=15
	};

	//! Values defining the blend operation used when blend is enabled
	enum E_BLEND_OPERATION
	{
		EBO_NONE = 0,	//!< No blending happens
		EBO_ADD,		//!< Default blending adds the color values
		EBO_SUBTRACT,	//!< This mode subtracts the color values
		EBO_REVSUBTRACT,//!< This modes subtracts destination from source
		EBO_MIN,		//!< Choose minimum value of each color channel
		EBO_MAX,		//!< Choose maximum value of each color channel
		EBO_COUNT
	};

	//!
	enum E_COLOR_LOGIC_OP
	{
	    ECLO_COPY=0,
	    ECLO_NOOP,
	    ECLO_COPY_INVERTED,
	    ECLO_INVERT,
	    ECLO_CLEAR,
	    ECLO_SET,
	    ECLO_AND,
	    ECLO_NAND,
	    ECLO_OR,
	    ECLO_XOR,
	    ECLO_EQUIV,
	    ECLO_AND_REVERSE,
	    ECLO_AND_INVERTED,
	    ECLO_OR_REVERSE,
	    ECLO_OR_INVERTED
	};


	//! Maximum number of texture an SMaterial can have.
	const uint32_t MATERIAL_MAX_TEXTURES = _IRR_MATERIAL_MAX_TEXTURES_;

#include "irrpack.h"
	//! Struct for holding parameters for a material renderer
	class SMaterial
	{
	public:
		//! Default constructor. Creates a solid, lit material with white colors
		SMaterial()
		: MaterialType(EMT_SOLID), AmbientColor(255,255,255,255), DiffuseColor(255,255,255,255),
			EmissiveColor(0,0,0,0), SpecularColor(255,255,255,255),
			Shininess(0.0f), MaterialTypeParam(0.0f), MaterialTypeParam2(0.0f), userData(NULL), Thickness(1.0f),
			ZBuffer(ECFN_GREATEREQUAL), ColorMask(ECP_ALL),
			BlendOperation(EBO_NONE),
			PolygonOffsetConstantMultiplier(0.f), PolygonOffsetGradientMultiplier(0.f),
			Wireframe(false), PointCloud(false), ZWriteEnable(true), BackfaceCulling(true), FrontfaceCulling(false), RasterizerDiscard(false)
		{ }

		//! Copy constructor
		/** \param other Material to copy from. */
		SMaterial(const SMaterial& other)
		{
			*this = other;
		}

		//! Assignment operator
		/** \param other Material to copy from. */
		SMaterial& operator=(const SMaterial& other)
		{
			// Check for self-assignment!
			if (this == &other)
				return *this;

			MaterialType = other.MaterialType;

			AmbientColor = other.AmbientColor;
			DiffuseColor = other.DiffuseColor;
			EmissiveColor = other.EmissiveColor;
			SpecularColor = other.SpecularColor;
			Shininess = other.Shininess;
			MaterialTypeParam = other.MaterialTypeParam;
			MaterialTypeParam2 = other.MaterialTypeParam2;
			userData = other.userData;
			Thickness = other.Thickness;
			for (uint32_t i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				TextureLayer[i] = other.TextureLayer[i];
			}

			Wireframe = other.Wireframe;
			PointCloud = other.PointCloud;
			ZWriteEnable = other.ZWriteEnable;
			BackfaceCulling = other.BackfaceCulling;
			FrontfaceCulling = other.FrontfaceCulling;
			RasterizerDiscard = other.RasterizerDiscard;
			ZBuffer = other.ZBuffer;
			ColorMask = other.ColorMask;
			BlendOperation = other.BlendOperation;
			PolygonOffsetConstantMultiplier = other.PolygonOffsetConstantMultiplier;
			PolygonOffsetGradientMultiplier = other.PolygonOffsetGradientMultiplier;

			return *this;
		}

		//! Texture layer array.
		SMaterialLayer TextureLayer[MATERIAL_MAX_TEXTURES];

		//! Type of the material. Specifies how everything is blended together
		E_MATERIAL_TYPE MaterialType;

		//! How much ambient light (a global light) is reflected by this material.
		/** The default is full white, meaning objects are completely
		globally illuminated. Reduce this if you want to see diffuse
		or specular light effects. */
		SColor AmbientColor;

		//! How much diffuse light coming from a light source is reflected by this material.
		/** The default is full white. */
		SColor DiffuseColor;

		//! Light emitted by this material. Default is to emit no light.
		SColor EmissiveColor;

		//! How much specular light (highlights from a light) is reflected.
		/** The default is to reflect white specular light. See
		SMaterial::Shininess on how to enable specular lights. */
		SColor SpecularColor;

		//! Value affecting the size of specular highlights.
		/** A value of 20 is common. If set to 0, no specular
		highlights are being used. To activate, simply set the
		shininess of a material to a value in the range [0.5;128]:
		\code
		sceneNode->getMaterial(0).Shininess = 20.0f;
		\endcode

		You can change the color of the highlights using
		\code
		sceneNode->getMaterial(0).SpecularColor.set(255,255,255,255);
		\endcode

		The specular color of the dynamic lights
		(SLight::SpecularColor) will influence the the highlight color
		too, but they are set to a useful value by default when
		creating the light scene node. Here is a simple example on how
		to use specular highlights:
		\code
		// load and display mesh
		scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(
		smgr->getMesh("data/faerie.md2"));
		node->setMaterialTexture(0, driver->getTexture("data/Faerie2.pcx")); // set diffuse texture
		node->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		node->getMaterial(0).Shininess = 20.0f; // set size of specular highlights
		\endcode */
		float Shininess;

		//! Free parameter, dependent on the material type.
		/** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
		and EMT_TRANSPARENT_ALPHA_CHANNEL. */
		float MaterialTypeParam;

		//! Second free parameter, dependent on the material type.
		/** Mostly ignored. */
		float MaterialTypeParam2;

		//! User Data
		void* userData;
		int32_t userData2_irrmat;			// sodan (this one is mine, MINE, MINE!!!)
		int32_t userData2_textureatlas;		// sodan (this one is mine, MINE, MINE!!!)

		//! Thickness of non-3dimensional elements such as lines and points.
		float Thickness;

		//! Is the ZBuffer enabled? Default: ECFN_GREATEREQUAL
		/** Values are from E_COMPARISON_FUNC. */
		uint8_t ZBuffer;

		//! Defines the enabled color planes
		/** Values are defined as or'ed values of the E_COLOR_PLANE enum.
		Only enabled color planes will be rendered to the current render
		target. Typical use is to disable all colors when rendering only to
		depth or stencil buffer, or using Red and Green for Stereo rendering. */
		uint8_t ColorMask:4;

		//! Store the blend operation of choice
		/** Values to be chosen from E_BLEND_OPERATION. The actual way to use this value
		is not yet determined, so ignore it for now. */
		E_BLEND_OPERATION BlendOperation:4;

		float PolygonOffsetConstantMultiplier;

		float PolygonOffsetGradientMultiplier;

		//! Draw as wireframe or filled triangles? Default: false
		/** The user can access a material flag using
		\code material.Wireframe=true \endcode
		or \code material.setFlag(EMF_WIREFRAME, true); \endcode */
		bool Wireframe:1;

		//! Draw as point cloud or filled triangles? Default: false
		bool PointCloud:1;

		//! Is the zbuffer writeable or is it read-only. Default: true.
		/** This flag is forced to false if the MaterialType is a
		transparent type and the scene parameter
		ALLOW_ZWRITE_ON_TRANSPARENT is not set. */
		bool ZWriteEnable:1;

		//! Is backface culling enabled? Default: true
		bool BackfaceCulling:1;

		//! Is frontface culling enabled? Default: false
		bool FrontfaceCulling:1;

		bool RasterizerDiscard:1;

		//! Gets the i-th texture
		/** \param i The desired level.
		\return Texture for texture level i, if defined, else 0. */
		IVirtualTexture* getTexture(uint32_t i) const
		{
			return i < MATERIAL_MAX_TEXTURES ? TextureLayer[i].Texture : 0;
		}

		//! Sets the i-th texture
		/** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
		\param i The desired level.
		\param tex Texture for texture level i. */
		void setTexture(uint32_t i, IVirtualTexture* tex)
		{
			if (i>=MATERIAL_MAX_TEXTURES)
				return;
			TextureLayer[i].Texture = tex;
		}

		//! Sets the Material flag to the given value
		/** \param flag The flag to be set.
		\param value The new value for the flag. */
		void setFlag(E_MATERIAL_FLAG flag, bool value)
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					Wireframe = value; break;
				case EMF_POINTCLOUD:
					PointCloud = value; break;
				case EMF_ZBUFFER:
					ZBuffer = value; break;
				case EMF_ZWRITE_ENABLE:
					ZWriteEnable = value; break;
				case EMF_BACK_FACE_CULLING:
					BackfaceCulling = value; break;
				case EMF_FRONT_FACE_CULLING:
					FrontfaceCulling = value; break;
				case EMF_COLOR_MASK:
					ColorMask = value?ECP_ALL:ECP_NONE; break;
				case EMF_BLEND_OPERATION:
					BlendOperation = value?EBO_ADD:EBO_NONE; break;
				default:
					break;
			}
		}

		//! Gets the Material flag
		/** \param flag The flag to query.
		\return The current value of the flag. */
		bool getFlag(E_MATERIAL_FLAG flag) const
		{
			switch (flag)
			{
				case EMF_WIREFRAME:
					return Wireframe;
				case EMF_POINTCLOUD:
					return PointCloud;
				case EMF_ZBUFFER:
					return ZBuffer!=ECFN_NEVER;
				case EMF_ZWRITE_ENABLE:
					return ZWriteEnable;
				case EMF_BACK_FACE_CULLING:
					return BackfaceCulling;
				case EMF_FRONT_FACE_CULLING:
					return FrontfaceCulling;
				case EMF_COLOR_MASK:
					return (ColorMask!=ECP_NONE);
				case EMF_BLEND_OPERATION:
					return BlendOperation != EBO_NONE;
			}

			return false;
		}

		//! Inequality operator
		/** \param b Material to compare to.
		\return True if the materials differ, else false. */
		inline bool operator!=(const SMaterial& b) const
		{
			bool different =
				MaterialType != b.MaterialType ||
				AmbientColor != b.AmbientColor ||
				DiffuseColor != b.DiffuseColor ||
				EmissiveColor != b.EmissiveColor ||
				SpecularColor != b.SpecularColor ||
				Shininess != b.Shininess ||
				MaterialTypeParam != b.MaterialTypeParam ||
				MaterialTypeParam2 != b.MaterialTypeParam2 ||
				userData != b.userData ||
				Thickness != b.Thickness ||
				Wireframe != b.Wireframe ||
				PointCloud != b.PointCloud ||
				ZBuffer != b.ZBuffer ||
				ZWriteEnable != b.ZWriteEnable ||
				BackfaceCulling != b.BackfaceCulling ||
				FrontfaceCulling != b.FrontfaceCulling ||
				RasterizerDiscard != b.RasterizerDiscard ||
				ColorMask != b.ColorMask ||
				BlendOperation != b.BlendOperation ||
				PolygonOffsetConstantMultiplier != b.PolygonOffsetConstantMultiplier ||
				PolygonOffsetGradientMultiplier != b.PolygonOffsetGradientMultiplier;
			for (uint32_t i=0; (i<MATERIAL_MAX_TEXTURES) && !different; ++i)
			{
				different |= (TextureLayer[i] != b.TextureLayer[i]);
			}
			return different;
		}

		//! Equality operator
		/** \param b Material to compare to.
		\return True if the materials are equal, else false. */
		inline bool operator==(const SMaterial& b) const
		{ return !(b!=*this); }

		bool isTransparent() const
		{
			return MaterialType==EMT_TRANSPARENT_ADD_COLOR ||
				MaterialType==EMT_TRANSPARENT_ALPHA_CHANNEL ||
				MaterialType==EMT_TRANSPARENT_VERTEX_ALPHA;
		}
	} PACK_STRUCT;

#include "irrunpack.h"

	//! global const identity Material
	IRRLICHT_API extern SMaterial IdentityMaterial;



	//!
	class RasterStencilDepthState
	{
        public:
            uint32_t EnableDepthClamp           : 1;

            //! Is the ZBuffer enabled? Default: ECFN_GREATEREQUAL
            /** Values are from E_COMPARISON_FUNC. */
            uint32_t DepthComparisonFunc        : 3;
            //! Is the zbuffer writeable or is it read-only. Default: true.
            uint32_t WriteToZBuffer             : 1;

            //! How is the polygon filled.
            /** Only matters if you're drawing higher dimensional polygons. */
            uint32_t PolygonRasterModeFront     : 2;
            uint32_t PolygonRasterModeBack      : 2;
            uint32_t PolygonFrontFaceCW         : 1;
            uint32_t PolygonCullFace            : 2;

            //! What are the operations to carry out on different test outcomes. Default: ESF_KEEP
            /** Values are from E_STENCIL_FUNC. */
            uint32_t StencilFrontOpStencilFail  : 3;
            uint32_t StencilFrontOpDepthFail    : 3;
            uint32_t StencilFrontOpDepthPass    : 3;
            //! What's the stencil func for fragment to pass?
            /** (ref&mask) FUNC (frameBufferValue&mask) */
            uint32_t StencilFrontFunc           : 3;
            uint32_t StencilBackFunc            : 3;

            //! separate 32bit int
            uint32_t StencilFrontRef            : 8;
            uint32_t StencilFrontMask           : 8;
            uint32_t StencilBackRef             : 8;
            uint32_t StencilBackMask            : 8;
	};

    //! Blend Stuff
    class BlendState
    {
        public:
            enum E_BLEND_OPERATION
            {
                EBO_ADD = 0,		//!< Default blending adds the color values
                EBO_SUBTRACT,	//!< This mode subtracts the color values
                EBO_REVSUBTRACT,//!< This modes subtracts destination from source
                EBO_MIN,		//!< Choose minimum value of each color channel
                EBO_MAX,		//!< Choose maximum value of each color channel
                EBO_COUNT
            };

            enum E_BLEND_FACTOR
            {
                EBF_ZERO = 0,
                EBF_ONE,
                EBF_SRC_COLOR,
                EBF_ONE_MINUS_SRC_COLOR,
                EBF_DST_COLOR,
                EBF_ONE_MINUS_DST_COLOR,
                EBF_SRC_ALPHA,
                EBF_ONE_MINUS_SRC_ALPHA,
                EBF_DST_ALPHA,
                EBF_ONE_MINUS_DST_ALPHA,
                EBF_CONSTANT_COLOR,
                EBF_ONE_MINUS_CONSTANT_COLOR,
                EBF_CONSTANT_ALPHA,
                EBF_ONE_MINUS_CONSTANT_ALPHA,
                EBF_SRC_ALPHA_SATURATE,
                EBF_SRC1_COLOR,
                EBF_ONE_MINUS_SRC1_COLOR,
                EBF_SRC1_ALPHA,
                EBF_ONE_MINUS_SRC1_ALPHA,
                EBF_COUNT
            };

            BlendState()
            {
                ColorMask       = ECP_ALL;

                OperationRGB    = EBO_ADD;
                OperationA      = EBO_ADD;

                FactorSrcRGB    = EBF_ONE;
                FactorDstRGB    = EBF_ZERO;
                FactorSrcA      = EBF_ONE;
                FactorDstA      = EBF_ZERO;
            }

            inline void collapseAmbigousDesc()
            {
                if (OperationRGB==EBO_MIN||OperationRGB==EBO_MAX)
                {
                    FactorSrcRGB = EBF_ONE;
                    FactorDstRGB = EBF_ONE;
                }

                if (OperationA==EBO_MIN||OperationA==EBO_MAX)
                {
                    FactorSrcA = EBF_ONE;
                    FactorDstA = EBF_ONE;
                }

                FactorSrcA = collapseAlphaFactor(FactorSrcA);
                FactorDstA = collapseAlphaFactor(FactorDstA);
            }

            inline bool requiresBlendEnable() const
            {
                bool retval;

                if (ColorMask&ECP_RGB)
                {
                    switch (OperationRGB)
                    {
                        case EBO_ADD:
                            retval = !(FactorSrcRGB==EBF_ONE&&FactorDstRGB==EBF_ZERO);
                            break;
                        case EBO_SUBTRACT:
                            retval = !(FactorSrcRGB==EBF_ONE&&FactorDstRGB==EBF_ZERO);
                            break;
                        default:
                            retval = true;
                    }
                }
                else
                    retval = false;

                if ((ColorMask&ECP_ALPHA)&&(!retval))
                {
                    switch (OperationA)
                    {
                        case EBO_ADD:
                            retval = !((FactorSrcA==EBF_ONE||FactorSrcA==EBF_SRC_ALPHA_SATURATE)&&FactorDstA==EBF_ZERO);
                            break;
                        case EBO_SUBTRACT:
                            retval = !((FactorSrcA==EBF_ONE||FactorSrcA==EBF_SRC_ALPHA_SATURATE)&&FactorDstA==EBF_ZERO);
                            break;
                        default:
                            retval = true;
                    }
                }


                return retval;
            }

            inline bool careAboutBlendState() const
            {
                return ColorMask;
            }


            uint32_t ColorMask      : 4;

            uint32_t OperationRGB   : 3;
            uint32_t OperationA     : 3;

            uint32_t FactorSrcRGB   : 5;
            uint32_t FactorDstRGB   : 5;
            uint32_t FactorSrcA     : 5;
            uint32_t FactorDstA     : 5;
        private:
            static uint32_t collapseAlphaFactor(uint32_t factor)
            {
                switch (factor)
                {
                    case EBF_SRC_COLOR:
                        return EBF_SRC_ALPHA;
                        break;
                    case EBF_ONE_MINUS_SRC_COLOR:
                        return EBF_ONE_MINUS_SRC_ALPHA;
                        break;
                    case EBF_DST_COLOR:
                        return EBF_DST_ALPHA;
                        break;
                    case EBF_ONE_MINUS_DST_COLOR:
                        return EBF_ONE_MINUS_DST_ALPHA;
                        break;
                    case EBF_CONSTANT_COLOR:
                        return EBF_CONSTANT_ALPHA;
                        break;
                    case EBF_ONE_MINUS_CONSTANT_COLOR:
                        return EBF_ONE_MINUS_CONSTANT_ALPHA;
                        break;
                    case EBF_SRC_ALPHA_SATURATE:
                        return EBF_ONE;
                        break;
                    case EBF_SRC1_COLOR:
                        return EBF_SRC1_ALPHA;
                        break;
                    case EBF_ONE_MINUS_SRC1_COLOR:
                        return EBF_ONE_MINUS_SRC1_ALPHA;
                        break;
                    default:
                        break;
                }

                return factor;
            }
    };

    class RangedBufferMapping
    {
        public:
            RangedBufferMapping() : buffer(NULL), offset(0), size(0)
            {
            }
            RangedBufferMapping(IGPUBuffer* inBuffer, const uint32_t& inOffset, const uint32_t& inSize)
                : buffer(NULL), offset(0), size(0)
            {
                if (!inBuffer)
                    return;

                uint32_t bufSz = inBuffer->getSize();
                if (inOffset>bufSz||inSize>bufSz)
                    return;

                buffer  = inBuffer;
                offset  = inOffset;
                size    = inSize;
            }

            IGPUBuffer* buffer;
            uint32_t    offset,size;
    };

    enum E_RASTER_SHADER_STAGE
    {
        ERSS_VERTEX=0,
        ERSS_TESS_CONTROL,
        ERSS_TESS_EVALUATION,
        ERSS_GEOMETRY,
        ERSS_FRAGMENT,
        ERSS_COUNT
    };

/*
	//! Struct for holding parameters for a material renderer
	class CMaterial
	{
	public:

		//! Default constructor. Creates a solid, lit material with white colors
		CMaterial()
          : AmbientColor(1.f), DiffuseColor(1.f), EmissiveColor(0.f), SpecularColor(1.f),
			RoughnessVariable(0.0f), MaterialTypeParam(0.0f), MaterialTypeParam2(0.0f), userData(NULL), Thickness(1.0f),
			ZBuffer(ECFN_GREATEREQUAL), ColorMask(ECP_ALL),
			BlendOperation(EBO_NONE),
			PolygonOffsetConstantMultiplier(0.f), PolygonOffsetGradientMultiplier(0.f),
			Wireframe(false), PointCloud(false), ZWriteEnable(true), BackfaceCulling(true), FrontfaceCulling(false), RasterizerDiscard(false)
		{
		}

		//! Copy constructor
		/** \param other Material to copy from. *
		CMaterial(const CMaterial& other)
		{
			*this = other;
		}

		//! Assignment operator
		/** \param other Material to copy from. *
		CMaterial& operator=(const CMaterial& other)
		{
			// Check for self-assignment!
			if (this == &other)
				return *this;

			MaterialType = other.MaterialType;

			AmbientColor = other.AmbientColor;
			DiffuseColor = other.DiffuseColor;
			EmissiveColor = other.EmissiveColor;
			SpecularColor = other.SpecularColor;
			RoughnessVariable = other.RoughnessVariable;
			MaterialTypeParam = other.MaterialTypeParam;
			MaterialTypeParam2 = other.MaterialTypeParam2;
			userData = other.userData;
			Thickness = other.Thickness;
			for (uint32_t i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				TextureLayer[i] = other.TextureLayer[i];
			}

			Wireframe = other.Wireframe;
			PointCloud = other.PointCloud;
			ZWriteEnable = other.ZWriteEnable;
			BackfaceCulling = other.BackfaceCulling;
			FrontfaceCulling = other.FrontfaceCulling;
			RasterizerDiscard = other.RasterizerDiscard;
			ZBuffer = other.ZBuffer;
			ColorMask = other.ColorMask;
			BlendOperation = other.BlendOperation;
			PolygonOffsetConstantMultiplier = other.PolygonOffsetConstantMultiplier;
			PolygonOffsetGradientMultiplier = other.PolygonOffsetGradientMultiplier;

			return *this;
		}

		// NON GL-STATE MEMBERS

		//! How much ambient light (a global light) is reflected by this material.
		/** The default is full white, meaning objects are completely
		globally illuminated. Reduce this if you want to see diffuse
		or specular light effects. *
		vectorSIMDf AmbientColor;

		//! How much diffuse light coming from a light source is reflected by this material.
		/** The default is full white. *
		vectorSIMDf DiffuseColor;

		//! Light emitted by this material. Default is to emit no light.
		vectorSIMDf EmissiveColor;

		//! How much specular light (highlights from a light) is reflected.
		/** The default is to reflect white specular light. See
		SMaterial::Shininess on how to enable specular lights. *
		vectorSIMDf SpecularColor;

		//!
		float RoughnessVariable;

		//! Free parameter, dependent on the material type.
		/** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
		and EMT_TRANSPARENT_ALPHA_CHANNEL. *
		float MaterialTypeParam;

		//! Second free parameter, dependent on the material type.
		/** Mostly ignored. *
		float MaterialTypeParam2;

		//! Thickness of non-3dimensional elements such as lines and points.
		float Thickness;

		float PolygonOffsetConstantMultiplier;
		float PolygonOffsetGradientMultiplier;

		//! User Data
		int32_t userData2_irrmat;			// sodan (this one is mine, MINE, MINE!!!)
		int32_t userData2_textureatlas;		// sodan (this one is mine, MINE, MINE!!!)
		void* userData;
		void* uncopyableUserData;


		//! Texture layer array.
		SMaterialLayer TextureLayer[MATERIAL_MAX_TEXTURES];

		//! UBOs
		#define MATERIAL_MAX_UNIFORM_BUFFER_OBJECTS 6
		RangedBufferMapping ShaderUniformBuffers[MATERIAL_MAX_UNIFORM_BUFFER_OBJECTS];

		//! Uniform Subroutines
		#define MATERIAL_MAX_SUBROUTINES_PER_STAGE 32
		uint8_t ShaderSubroutineFuncIx[ERSS_COUNT][MATERIAL_MAX_SUBROUTINES_PER_STAGE]; // THIS WILL BE 16BYTE ALIGNED

		inline bool needToResetSubroutines(const E_RASTER_SHADER_STAGE& stage, const CMaterial& other)
		{
#ifdef _DEBUG
            if (stage>=ERSS_COUNT)
                return false;
#endif

#ifdef _IRR_WINDOWS_
            __declspec(align(32)) uint64_t result[4];
#else
            uint64_t result[4] __attribute__ ((__aligned__(32)));
#endif

#ifdef AVX
            __m256 ymm0 = _mm256_loadu_ps(ShaderSubroutineFuncIx[stage]);
            __m256 ymm1 = _mm256_loadu_ps(other.ShaderSubroutineFuncIx[stage]);
            _mm256_store_ps((float*)result,_mm256_xor_ps(ymm0,ymm1));
#else
            __m128 xmm0 = _mm_load_ps(ShaderSubroutineFuncIx[stage]);
            __m128 xmm1 = _mm_load_ps(ShaderSubroutineFuncIx[stage]+16);
            __m128 xmm2 = _mm_load_ps(other.ShaderSubroutineFuncIx[stage]);
            __m128 xmm3 = _mm_load_ps(other.ShaderSubroutineFuncIx[stage]+16);
            _mm_store_ps((float*)(result+0),_mm_xor_ps(xmm0,xmm1));
            _mm_store_ps((float*)(result+2),_mm_xor_ps(xmm2,xmm3));
#endif
            return result[0]|result[1]|result[2]|result[3];
		}


		// GL-STATE VARS

        //! Type of the material. Specifies how everything is blended together
        uint32_t ShaderMaterial             : 16; // you don't need more shaders
        uint32_t EnableAlphaToCoverage      : 1;
        uint32_t EnableAlphaToOne           : 1;
        //!
        uint32_t ColorLogicOp               : 4;

        RasterStencilDepthState rasterStencilDepthState;

        BlendState attachmentBlendStates[OGL_STATE_MAX_DRAW_BUFFERS];


        //! Use negative values to mean invert (pay attention to 0)
        float MultisampleCoverage;

        //! Set to 0.f to only shade one sample per pixel (default) and 1.f to full SSAA
        float MinSampleShadingRate;

        //! All Enabled
        uint32_t SampleMask[OGL_STATE_MAX_SAMPLE_MASK_WORDS];


		//! Inequality operator
		/** \param b Material to compare to.
		\return True if the materials differ, else false. *
		inline bool operator!=(const SMaterial& b) const
		{
			bool different =
				MaterialType != b.MaterialType ||
				AmbientColor != b.AmbientColor ||
				DiffuseColor != b.DiffuseColor ||
				EmissiveColor != b.EmissiveColor ||
				SpecularColor != b.SpecularColor ||
				Shininess != b.Shininess ||
				MaterialTypeParam != b.MaterialTypeParam ||
				MaterialTypeParam2 != b.MaterialTypeParam2 ||
				userData != b.userData ||
				Thickness != b.Thickness ||
				Wireframe != b.Wireframe ||
				PointCloud != b.PointCloud ||
				ZBuffer != b.ZBuffer ||
				ZWriteEnable != b.ZWriteEnable ||
				BackfaceCulling != b.BackfaceCulling ||
				FrontfaceCulling != b.FrontfaceCulling ||
				RasterizerDiscard != b.RasterizerDiscard ||
				ColorMask != b.ColorMask ||
				BlendOperation != b.BlendOperation ||
				PolygonOffsetConstantMultiplier != b.PolygonOffsetConstantMultiplier ||
				PolygonOffsetGradientMultiplier != b.PolygonOffsetGradientMultiplier;
			for (uint32_t i=0; (i<MATERIAL_MAX_TEXTURES) && !different; ++i)
			{
				different |= (TextureLayer[i] != b.TextureLayer[i]);
			}
			return different;
		}

		//! Equality operator
		/** \param b Material to compare to.
		\return True if the materials are equal, else false. *
		inline bool operator==(const SMaterial& b) const
		{ return !(b!=*this); }
	};
*/

} // end namespace video
} // end namespace irr

#endif
