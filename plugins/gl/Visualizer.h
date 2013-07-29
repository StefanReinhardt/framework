//
// very simple utility class for making drawing of debug stuff easier
//
#pragma once
#include <map>
#include <memory>

#include "Shader.h"
#include "Geometry.h"
#include "Context.h"
/*
#include "gfx/Geometry.h"

#include "gfx/Camera.h"
//#include <gfx/Field.h>
*/

namespace gl
{
	class Visualizer
	{
	public:
		typedef std::shared_ptr<Visualizer> Ptr;

		typedef int Handle;

		struct Primitive
		{
			typedef std::shared_ptr<Primitive> Ptr;
			Primitive();
			virtual                              ~Primitive();
			virtual void                          preRender();
			virtual void                          render( gl::Context* context );
			void                                  hide();
			void                                  show();
			void                                  setTransform( const math::Matrix44f &xform );
			void                                  setColor( float r, float g, float b );
			void                                  setColor( const math::Vec3f &color );

			gl::Geometry::Ptr                     m_geo;
			gl::Shader::Ptr                       m_shader;
			math::Matrix44f                       m_xform;
			bool                                  m_pointSprites;
			bool                                  m_visible;
		};
		/*
		BASE_DECL_SMARTPTR_STRUCT(Point);
		struct Point : public Primitive
		{
			math::Vec3f                             position();
			void      setPosition( float x, float y, float z );
			void           setPosition( const math::Vec3f &p );
			math::Vec3f                                color();
			void         setColor( float r, float g, float b );
		private:
		};

		BASE_DECL_SMARTPTR_STRUCT(Line);
		struct Line : public Primitive
		{
			void            setP1( float x, float y, float z );
			void                setP1( const math::Vec3f &p1 );
			math::Vec3f                                getP1();
			void            setP2( float x, float y, float z );
			void                setP2( const math::Vec3f &p2 );
			math::Vec3f                                getP2();
		private:
			math::Vec3f                             m_p1, m_p2;
		};

		BASE_DECL_SMARTPTR_STRUCT(Lines);
		struct Lines : public Primitive
		{
			unsigned int  add( const math::V3f &p1, const math::V3f &p2 );
			void                setColor( int i, const math::V3f &color );
			void                                                  clear();
		};

		BASE_DECL_SMARTPTR_STRUCT(Points);
		struct Points : public Primitive
		{
			Points();
			unsigned int                        add( const math::V3f &p );
			void                setColor( int i, const math::V3f &color );
			void                                                  clear();
			void                               setPointSize( float size );
		protected:
			virtual void                                      preRender();
			float                                             m_pointSize;
		};

		BASE_DECL_SMARTPTR_STRUCT(Circle);
		struct Circle : public Primitive
		{
			math::Vec3f                            getCenter();
			void        setCenter( float x, float y, float z );
			void        setCenter( const math::Vec3f &center );
			math::Vec3f                                color();
			void         setColor( float r, float g, float b );
			float                                  getRadius();
			void                          setRadius( float r );
		private:
			float                                     m_radius;
			math::Vec3f                               m_center;
		};

		BASE_DECL_SMARTPTR_STRUCT(Cone);
		struct Cone : public Primitive
		{
			Cone( const math::Vec3f &apex, const math::Vec3f &axis, float halfAngle, float height );
			math::Vec3f                              getApex();
			void          setApex( float x, float y, float z );
			math::Vec3f                              getAxis();
			void          setAxis( float x, float y, float z );
			void            setAxis( const math::Vec3f &axis );
			float                               getHalfAngle();
			void                       setHalfAngle( float a );
			float                                  getHeight();
			void                          setHeight( float h );

		private:
			void                                  regenerate();
			math::Vec3f                                 m_apex;
			math::Vec3f                                 m_axis;
			float                                  m_halfAngle;
			float                                     m_height;
		};

		BASE_DECL_SMARTPTR_STRUCT(Box);
		struct Box : public Primitive
		{
			Box();
			void set( const math::BoundingBox3f &bound );
		};

		BASE_DECL_SMARTPTR_STRUCT(Sphere);
		struct Sphere : public Primitive
		{
			Sphere();
			float                                     m_radius;
		};


		BASE_DECL_SMARTPTR_STRUCT(Frustum);
		struct Frustum : public Primitive
		{
			Frustum( float fov, float nearz, float farz, float aspect );

			float                                     getFov();
			void                           setFov( float fov );
			float                                   getNearZ();
			void                       setNearZ( float nearz );
			float                                    getFarZ();
			void                         setFarZ( float farz );
			float                                  getAspect();
			void                     setAspect( float aspect );
			void        setOrigin( const math::Vec3f &origin );
			math::Vec3f                            getOrigin();
			void        setDirection( const math::Vec3f &dir );
			math::Vec3f                         getDirection();

			void                                updatePoints();
		private:
			float                                        m_fov;
			float                                      m_nearz;
			float                                       m_farz;
			float                                     m_aspect;
			math::Vec3f                               m_origin;
			math::Vec3f                            m_direction;
		};

		BASE_DECL_SMARTPTR_STRUCT(BaseVectors);
		struct BaseVectors : public Primitive
		{
			BaseVectors( float scale = 1.0f );
			void                 setScale( const float scale );
			float                              getScale()const;
		private:
			float                                      m_scale;
		};

		BASE_DECL_SMARTPTR_STRUCT(Grid);
		struct Grid : public Primitive
		{
			Grid();
		private:
		};


		BASE_DECL_SMARTPTR_STRUCT(Camera);
		struct Camera : public Frustum
		{
			Camera( gfx::CameraPtr cam );
		private:
			virtual void           preRender();
			gfx::CameraPtr              m_cam;
		};

	//	BASE_DECL_SMARTPTR_STRUCT(VectorField);
	//	struct VectorField : public Primitive
	//	{
	//		VectorField( Field3f::Ptr field );
	//		void                             setMode( int mode );
	//		void                                        update();
	//		void        setShowCenters( bool showCenters = true);
	//		void                   setVectorScale( float scale );
	//		void   setVectorLengthCap( bool enabled, float max );
	//	private:
	//		virtual void                             preRender();
	//		virtual void            render( ContextPtr context );

	//		Field3f::Ptr                                 m_field;

	//		int                                           m_mode; // vectors/streaklines

	//		math::V3i                               m_resolution; // current resolution
	//		Geometry::Ptr                                m_vectors; //
	//		Geometry::Ptr                                m_centers; //
	//		bool                                   m_showCenters;
	//		float                                  m_vectorScale;
	//		bool                        m_vectorLengthCapEnabled;
	//		float                              m_vectorLengthCap;
	//		float                           m_vectorLengthCapSqr;
	//	};


	//	BASE_DECL_SMARTPTR_STRUCT(ScalarField);
	//	struct ScalarField : public Primitive
	//	{
	//		ScalarField( Fieldf::Ptr field );
	//		void                                    update();
	//	private:
	//		virtual void                         preRender();
	//		virtual void        render( ContextPtr context );
	//		Fieldf::Ptr                              m_field;
	//		math::V3i                           m_resolution; // current resolution
	//		Geometry::Ptr                            m_centers; //
	//	};


		// todo: ray
	*/
		Visualizer();                                    // constructor
		static Visualizer::Ptr                           create();

		void                                             render( Context* context );

		// primitves ---
		Primitive::Ptr                                   geometry( Geometry::Ptr geo, Shader::Ptr shader = Shader::Ptr() );


	/*
		PrimitivePtr                                                                   getPrimitive( const std::string &key );
		bool                                                                           hasPrimitive( const std::string &key );
		PrimitivePtr                                                                      primitive( const std::string &key );


		PointPtr                                                                           point( float x, float y, float z );
		PointPtr                                                                              point( const math::Vec3f &pos );
		LinePtr                                                          line( const math::Vec3f &p1, const math::Vec3f &p2 );
		PointsPtr                                                                                                    points();
		PointsPtr                                                                              points( const std::string &id);
		LinesPtr                                                                                                      lines();
		LinesPtr                                                                                lines( const std::string &id);
		CirclePtr                                                                                           circle( float r );
		ConePtr                       cone( const math::Vec3f &apex, const math::Vec3f &axis, float halfAngle, float height );
		BoxPtr                                                                        box( const math::BoundingBox3f &bound );
		BoxPtr                                                box( const std::string &id,  const math::BoundingBox3f &bound );
		SpherePtr                                                                                      sphere( float radius );
		FrustumPtr                                                frustum( float fov, float nearz, float farz, float aspect );
		BaseVectorsPtr                                                                      baseVectors( float scale = 1.0f );
		BaseVectorsPtr       baseVectors( const math::V3f &v0, const math::V3f &v1, const math::V3f &v2, float scale = 1.0f );
		GridPtr                                                                             grid( int xres = 5, int zres = 5);
		void                                                                                    camera( gfx::CameraPtr cam );
		//VectorFieldPtr                                          field( Field3f::Ptr vectorField, const std::string &id = "" );
		//VectorFieldPtr                                                                         field( const std::string &id );

		*/
	//private:
		std::vector<Primitive::Ptr>                     m_primitives;
		std::map<std::string, Primitive::Ptr>           m_primitiveIds;


		gl::Shader::Ptr                                 m_defaultShader;
	};

}
