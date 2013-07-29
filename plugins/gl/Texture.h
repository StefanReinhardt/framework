#pragma once

#include <memory>
#include "Attribute.h"
//#include "Image.h"

namespace gl
{
/*
	BASE_DECL_SMARTPTR_STRUCT(Texture1d);

	struct Texture1d
	{
		Texture1d();
		~Texture1d();

		void       uploadR8( int xres, const unsigned char *pixels );
		void          uploadRGBA8( int xres, unsigned char *pixels );
		void            uploadRGBAFloat32( int xres, float *pixels );
		void                uploadFloat32( int xres, float *pixels );
		AttributePtr                                    getUniform();


		unsigned int                                            m_id; // opengl handle
		AttributePtr                                       m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int										              m_xres; // texture info just for the record
		int                                          m_textureFormat; // format of texture in gpu memory

		static Texture1dPtr create( int textureFormat, int xres = 64 );
		static Texture1dPtr createR8( int xres = 64 );
		static Texture1dPtr createRGBA8( int xres = 64 );
		static Texture1dPtr createFloat32( int xres = 64 );
		static Texture1dPtr createRGBAFloat32( int xres = 64 );
	};
*/
	class Texture2d : public Attribute
	{
	public:
		typedef std::shared_ptr<Texture2d> Ptr;

		Texture2d( bool multisampled = false, int numSamples = 4 );
		~Texture2d();

		virtual void              bindAsUniform( int index )override;
		virtual void              unbindAsUniform( int index )override;


		/*
		int                       width();
		int                       height();
		void                      resize( int newWidth, int newHeight );

		void                      uploadRGBA8( int xres, int yres, unsigned char *pixels );
		void                      uploadRGBAFloat32( int xres, int yres, float *pixels );
		void                      uploadRGBFloat32( int xres, int yres, float *pixels );
		void                      upload( ImagePtr image );
		//ImagePtr                  download( int format, int componentType ); // e.g. GL_RGBA, GL_FLOAT
		AttributePtr              getUniform();


		AttributePtr              m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound



		static Texture2d::Ptr     createSRGB8( int xres = 64, int yres = 64 );
		static Texture2d::Ptr     createFloat16( int xres = 64, int yres = 64 );
		static Texture2d::Ptr     createRGBAFloat32( int xres = 64, int yres = 64, bool multisampled = false, int numSamples = 4 );
		static Texture2d::Ptr     createRGBAFloat16( int xres = 64, int yres = 64 );
		static Texture2d::Ptr     createRGBFloat32( int xres = 64, int yres = 64 );


		static Texture2d::Ptr     load( const Path &file, int format = 0x8058 ); // loads texture from file (0x8058=GL_RGBA8)
		*/
		void                      uploadFloat32( int xres, int yres, float *pixels );
		void                      uploadRGBA8( int xres, int yres, unsigned char *pixels );

		static Texture2d::Ptr     createFloat32();
		static Texture2d::Ptr     createRGBA8();
		static Texture2d::Ptr     createUVRefTexture(); // reference  texture
	private:
		static Texture2d::Ptr     create(int textureFormat, bool multisampled = false, int numSamples = 4 );

		unsigned int              m_id; // opengl handle
		int                       m_target; // this is either GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
		bool                      m_multiSampled; // used for multisampling
		int                       m_numSamples;
		int                       m_xres, m_yres; // texture info just for the record
		int                       m_textureFormat; // format of texture in gpu memory

	};




/*
	BASE_DECL_SMARTPTR_STRUCT(Texture3d);

	struct Texture3d
	{
		Texture3d();
		~Texture3d();

		void              uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels );
		void                uploadRGBAFloat32( int xres, int yres, int zres, float *pixels );
		void                    uploadFloat32( int xres, int yres, int zres, float *pixels );
		float                                                                    *download();
		AttributePtr                                                            getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                           m_xres, m_yres, m_zres; // texture info just for the record
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static Texture3dPtr create( int textureFormat, int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBA8( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createFloat16( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBAFloat16( int xres = 64, int yres = 64, int zres = 64 );
		static Texture3dPtr createRGBAFloat32( int xres = 64, int yres = 64, int zres = 64 );
	};

	BASE_DECL_SMARTPTR_STRUCT(Texture2dArray);

	struct Texture2dArray
	{
		Texture2dArray();
		~Texture2dArray();

		void   upload( int xres, int yres, int zres, int format, int component, void *data );
		void              uploadRGBA8( int xres, int yres, int zres, unsigned char *pixels );
		void                uploadRGBAFloat32( int xres, int yres, int zres, float *pixels );
		void                    uploadFloat32( int xres, int yres, int zres, float *pixels );
		AttributePtr                                                            getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                           m_xres, m_yres, m_zres; // texture info just for the record
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static Texture2dArrayPtr create( int textureFormat, int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBA8( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createDepth24( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBAFloat32( int xres = 64, int yres = 64, int zres = 64 );
		static Texture2dArrayPtr createRGBAFloat16( int xres = 64, int yres = 64, int zres = 64 );
	};

	BASE_DECL_SMARTPTR_STRUCT(TextureCube);

	struct TextureCube
	{
		TextureCube();
		~TextureCube();

		void                                                         upload( ImagePtr image ); // assumes Image contains all 6 faces
		void                                                      upload( ImagePtr faces[6] ); // assumes Image contains all 6 faces
		AttributePtr                                                             getUniform();


		unsigned int                                                                    m_id; // opengl handle
		AttributePtr                                                               m_uniform; // will be used to attach textures to geometry/or shaders which then will be bound
		int                                                                  m_textureFormat; // format of texture in gpu memory

		static TextureCubePtr                      createRGBA8(int xres = 64, int yres = 64);
	};
*/
}
