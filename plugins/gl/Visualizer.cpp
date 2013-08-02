//
// very simple utility class for making drawing of debug stuff easier
//
#include "Visualizer.h"

namespace gl
{
	Visualizer::Ptr Visualizer::create()
	{
		return Visualizer::Ptr( new Visualizer() );
	}

	Visualizer::Visualizer()
	{
		m_defaultShader = gl::Shader::createSimpleColorShader();
	}


	void Visualizer::render( Context* context )
	{
		for( auto it = m_primitives.begin(), end=m_primitives.end();it != end;++it )
		{
			Primitive &p = *(*it);

			if( p.m_visible )
			{
				p.preRender();
				p.render( context );
			}
		}
	}

	void Visualizer::setPrimitiveName( Primitive::Ptr prim, const std::string &name )
	{
		m_primitiveIds[name] = prim;
	}

	Visualizer::Primitive::Ptr Visualizer::getPrimitive( const std::string &name )
	{
		std::map<std::string, Primitive::Ptr>::iterator it = m_primitiveIds.find(name);
		if( it != m_primitiveIds.end() )
			return it->second;
		return Visualizer::Primitive::Ptr();
	}

	bool Visualizer::hasPrimitive( const std::string &name )
	{
		std::map<std::string, Primitive::Ptr>::iterator it = m_primitiveIds.find(name);
		if( it != m_primitiveIds.end() )
			return true;
		return false;
	}

	// PRIMITIVE =====================================
	Visualizer::Primitive::Primitive():m_pointSprites(false), m_xform(math::Matrix44f::Identity()), m_visible(true)
	{
	}

	Visualizer::Primitive::~Primitive()
	{
	}

	void Visualizer::Primitive::preRender()
	{
	}

	void Visualizer::Primitive::render( Context* context )
	{
		context->render( m_geo, m_shader, m_xform );
		//context->render(std::make_shared<gl::Geometry>( createQuad() ), m_shader);

	}

	void Visualizer::Primitive::hide()
	{
		m_visible=false;
	}


	void Visualizer::Primitive::show()
	{
		m_visible=true;
	}

	void Visualizer::Primitive::setTransform( const math::Matrix44f &xform )
	{
		m_xform = xform;
	}

	void Visualizer::Primitive::setColor( float r, float g, float b )
	{
		Attribute::Ptr cdAttr = m_geo->getAttr("Cd");
		int numElements = cdAttr->numElements();
		for( int i=0;i<numElements;++i )
			cdAttr->set( i, math::V3f(r, g, b) );
	}

	void Visualizer::Primitive::setColor( const math::Vec3f &color )
	{
		setColor( color.x, color.y, color.z );
	}


	// GEOMETRY ==========================================

	Visualizer::Primitive::Ptr Visualizer::geometry( Geometry::Ptr geo, Shader::Ptr shader )
	{
		Primitive::Ptr prim = std::make_shared<Primitive>();
		prim->m_geo = geo;
		if( shader )
			prim->m_shader = shader;
		else
			prim->m_shader = m_defaultShader;

		m_primitives.push_back(prim);

		return prim;
	}


	// Points =====================================

	Visualizer::Points::Points() : Primitive()
	{
		m_pointSize = 4.0f;
	}

	Visualizer::Points::Ptr Visualizer::points()
	{
		Visualizer::Points::Ptr l = std::make_shared<Visualizer::Points>();

		gl::Geometry::Ptr geo = std::make_shared<gl::Geometry>(createPoints());
		geo->setAttr( "Cd", gl::Attribute::createV3f());

		l->m_geo = geo;
		l->m_shader = m_defaultShader;
		m_primitives.push_back(l);

		return l;
	}


	unsigned int Visualizer::Points::add( const math::V3f &p, const math::V3f &color )
	{
		unsigned int i = m_geo->getAttr("P")->appendElement<math::Vec3f>(p);
		m_geo->getAttr("Cd")->appendElement<math::Vec3f>(color);
		return m_geo->addPoint(i);
	}

	void Visualizer::Points::setPosition( int i, const math::V3f &pos )
	{
		m_geo->getAttr("P")->set<math::Vec3f>( i, pos);
	}

	void Visualizer::Points::setColor( int i, const math::V3f &color )
	{
		m_geo->getAttr("Cd")->set<math::Vec3f>( i, color);
	}

	void Visualizer::Points::clear()
	{
		m_geo->clear();
	}

	void Visualizer::Points::setPointSize( float size )
	{
		m_pointSize = size;
	}

	void Visualizer::Points::preRender()
	{
		glPointSize( m_pointSize );
	}


	/*


	Visualizer::PrimitivePtr Visualizer::primitive( const std::string &key )
	{
		return getPrimitive(key);
	}










	// Point =====================================
	Visualizer::PointPtr Visualizer::point( float x, float y, float z )
	{
		return point( math::Vec3f(x, y, z) );
	}

	Visualizer::PointPtr Visualizer::point( const math::Vec3f &pos )
	{
		Visualizer::PointPtr p = std::make_shared<Visualizer::Point>();

		gfx::Geometry::Ptr geo = gfx::Geometry::createPointGeometry();
		geo->setAttr( "Cd", gfx::Attribute::createVec3f());
		geo->getAttr("P")->appendElement<math::Vec3f>(pos);
		geo->getAttr("Cd")->appendElement<math::Vec3f>(math::Vec3f(1.0f, 1.0f, 1.0f));
		geo->addPoint(0);

		p->m_geo = geo;
		p->m_shader = m_defaultShader;
		//p->m_pointSprites = true;
		p->setColor( 1.0f, 0.0f, 0.0f );
		p->setPosition( pos );

		m_primitives.push_back(p);

		return p;
	}

	void Visualizer::Point::setColor( float r, float g, float b )
	{
		m_geo->getAttr("Cd")->set<math::Vec3f>(0, math::Vec3f(r, g, b));
	}

	void Visualizer::Point::setPosition( float x, float y, float z )
	{
		m_geo->getAttr("P")->set<math::Vec3f>(0, math::Vec3f(x, y, z));
	}

	void Visualizer::Point::setPosition( const math::Vec3f &p )
	{
		m_geo->getAttr("P")->set<math::Vec3f>(0, p);
	}

	math::Vec3f Visualizer::Point::color()
	{
		return m_geo->getAttr("Cd")->get<math::Vec3f>(0);
	}

	math::Vec3f Visualizer::Point::position()
	{
		return m_geo->getAttr("P")->get<math::Vec3f>(0);
	}

	// Line =======================================

	Visualizer::LinePtr Visualizer::line( const math::Vec3f &p1, const math::Vec3f &p2 )
	{
		Visualizer::LinePtr l = std::make_shared<Visualizer::Line>();

		gfx::Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
		geo->setAttr( "Cd", gfx::Attribute::createVec3f());
		geo->getAttr("P")->appendElement<math::Vec3f>(p1);
		geo->getAttr("P")->appendElement<math::Vec3f>(p2);
		geo->getAttr("Cd")->appendElement<math::Vec3f>(math::Vec3f(1.0f, 1.0f, 1.0f));
		geo->getAttr("Cd")->appendElement<math::Vec3f>(math::Vec3f(1.0f, 1.0f, 1.0f));
		geo->addLine(0, 1);

		l->m_geo = geo;
		l->m_shader = m_defaultShader;

		m_primitives.push_back(l);

		return l;
	}

	void Visualizer::Line::setP1( float x, float y, float z )
	{
		setP1( math::Vec3f(x, y, z) );
	}

	void Visualizer::Line::setP1( const math::Vec3f &p1 )
	{
		m_p1 = p1;
		m_geo->getAttr("P")->set<math::Vec3f>(0, p1);
	}

	math::Vec3f Visualizer::Line::getP1()
	{
		return m_p1;
	}

	void Visualizer::Line::setP2( float x, float y, float z )
	{
		setP2( math::Vec3f(x, y, z) );
	}

	void Visualizer::Line::setP2( const math::Vec3f &p2 )
	{
		m_p2 = p2;
		m_geo->getAttr("P")->set<math::Vec3f>(1, p2);
	}

	math::Vec3f Visualizer::Line::getP2()
	{
		return m_p2;
	}

	// Lines ======================================
	Visualizer::LinesPtr Visualizer::lines()
	{
		Visualizer::LinesPtr l = std::make_shared<Visualizer::Lines>();

		gfx::Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
		geo->setAttr( "Cd", gfx::Attribute::createVec3f());

		l->m_geo = geo;
		l->m_shader = m_defaultShader;

		m_primitives.push_back(l);

		return l;
	}

	Visualizer::LinesPtr Visualizer::lines( const std::string &id)
	{
		PrimitivePtr prim = getPrimitive( id );
		if( prim && std::dynamic_pointer_cast<Lines>(prim) )
			return std::dynamic_pointer_cast<Lines>(prim);
		LinesPtr l = lines();
		if( !id.empty() )
			m_primitiveIds[id] = l;
		return l;
	}


	unsigned int Visualizer::Lines::add( const math::V3f &p1, const math::V3f &p2 )
	{
		unsigned int i = m_geo->getAttr("P")->appendElement<math::Vec3f>(p1);
		m_geo->getAttr("P")->appendElement<math::Vec3f>(p2);
		m_geo->getAttr("Cd")->appendElement<math::Vec3f>(math::Vec3f(1.0f, 1.0f, 1.0f));
		m_geo->getAttr("Cd")->appendElement<math::Vec3f>(math::Vec3f(1.0f, 1.0f, 1.0f));
		return m_geo->addLine(i, i+1);
	}

	void Visualizer::Lines::setColor( int i, const math::V3f &color )
	{
		m_geo->getAttr("Cd")->set<math::Vec3f>( i*2, color);
		m_geo->getAttr("Cd")->set<math::Vec3f>( i*2+1, color);
	}

	void Visualizer::Lines::clear()
	{
		m_geo->clear();
	}



	// Circle =====================================
	Visualizer::CirclePtr Visualizer::circle( float r )
	{
		Visualizer::CirclePtr c = std::make_shared<Visualizer::Circle>();

		gfx::Geometry::Ptr geo = gfx::geo_circle( 30, 1.0f );
		int numPoints = geo->getAttr("P")->numElements();
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		c->m_geo = geo;
		c->m_shader = m_defaultShader;
		c->setCenter( math::Vec3f(0.0f, 0.0f, 0.0f) );
		c->setRadius(1.0f);

		m_primitives.push_back(c);

		return c;
	}

	void Visualizer::Circle::setRadius( float r )
	{
		m_radius = r;
		m_xform = math::Matrix44f::ScaleMatrix(m_radius)*math::Matrix44f::TranslationMatrix(m_center);
	}

	void Visualizer::Circle::setCenter( float x, float y, float z )
	{
		setCenter(math::Vec3f(x, y, z));
	}

	void Visualizer::Circle::setCenter( const math::Vec3f &center )
	{
		m_center = center;
		m_xform = math::Matrix44f::ScaleMatrix(m_radius)*math::Matrix44f::TranslationMatrix(m_center);
	}




	// Cone =======================================

	Visualizer::ConePtr Visualizer::cone( const math::Vec3f &apex, const math::Vec3f &axis, float halfAngle, float height  )
	{
		Visualizer::ConePtr c = std::make_shared<Visualizer::Cone>( apex, axis, halfAngle, height );
		c->m_shader = m_defaultShader;

		m_primitives.push_back(c);

		return c;
	}

	Visualizer::Cone::Cone( const math::Vec3f &apex, const math::Vec3f &axis, float halfAngle, float height )
	{
		m_apex = apex;
		m_axis = axis;
		m_halfAngle = halfAngle;
		m_height = height;
		regenerate();
	}

	math::Vec3f Visualizer::Cone::getApex()
	{
		return m_apex;
	}

	void Visualizer::Cone::setApex( float x, float y, float z )
	{
		m_apex = math::Vec3f(x, y, z);
	}

	math::Vec3f Visualizer::Cone::getAxis()
	{
		return m_axis;
	}

	void Visualizer::Cone::setAxis( float x, float y, float z )
	{
		setAxis(math::Vec3f(x, y, z));
	}

	void Visualizer::Cone::setAxis( const math::Vec3f &axis )
	{
		m_axis = axis;
		regenerate();
	}

	float Visualizer::Cone::getHalfAngle()
	{
		return m_halfAngle;
	}

	void Visualizer::Cone::setHalfAngle( float a )
	{
		m_halfAngle = a;
		regenerate();
	}

	float Visualizer::Cone::getHeight()
	{
		return m_height;
	}

	void Visualizer::Cone::setHeight( float h )
	{
		m_height = h;
		regenerate();
	}

	void Visualizer::Cone::regenerate()
	{
		gfx::Geometry::Ptr geo = gfx::geo_cone( m_axis, m_halfAngle, m_height );
		int numPoints = geo->getAttr("P")->numElements();
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		m_geo = geo;
	}


	// BOX =====================================================
	Visualizer::BoxPtr Visualizer::box( const math::BoundingBox3f &bound )
	{
		Visualizer::BoxPtr f = std::make_shared<Visualizer::Box>();

		gfx::Geometry::Ptr geo = gfx::geo_cube( bound, gfx::Geometry::LINE );
		gfx::Attribute::Ptr pAttr = geo->getAttr("P");
		int numPoints = pAttr->numElements();
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		f->m_geo = geo;
		f->m_shader = m_defaultShader;

		m_primitives.push_back(f);

		return f;
	}

	Visualizer::BoxPtr Visualizer::box( const std::string &id,  const math::BoundingBox3f &bound )
	{
		BoxPtr b;

		std::map<std::string, PrimitivePtr>::iterator it = m_primitiveIds.find(id);
		if( it != m_primitiveIds.end() )
		{
			b = core::dynamic_pointer_cast<Visualizer::Box>(it->second);
			b->set(bound);
		}
		else
		{
			b = box(bound);
			m_primitiveIds.insert( std::make_pair<std::string, PrimitivePtr>(id, b) );
		}

		return b;
	}


	Visualizer::Box::Box()
	{

	}

	void Visualizer::Box::set( const math::BoundingBox3f &bound )
	{
		gfx::Attribute::Ptr pAttr = m_geo->getAttr("P");

		pAttr->set<math::Vec3f>( 0, math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.maxPoint.z) );
		pAttr->set<math::Vec3f>( 1, math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
		pAttr->set<math::Vec3f>( 2, math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
		pAttr->set<math::Vec3f>( 3, math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.maxPoint.z) );

		pAttr->set<math::Vec3f>( 4, math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.minPoint.z) );
		pAttr->set<math::Vec3f>( 5, math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.minPoint.z) );
		pAttr->set<math::Vec3f>( 6, math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.minPoint.z) );
		pAttr->set<math::Vec3f>( 7, math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.minPoint.z) );
	}


	// SPHERE ==================================================================
	Visualizer::SpherePtr Visualizer::sphere( float radius )
	{
		Visualizer::SpherePtr f = std::make_shared<Visualizer::Sphere>();

		gfx::Geometry::Ptr geo = gfx::geo_sphere( 30, 30, radius, math::Vec3f(0.0f, 0.0f, 0.0f), gfx::Geometry::LINE );
		gfx::Attribute::Ptr pAttr = geo->getAttr("P");
		int numPoints = pAttr->numElements();
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		f->m_geo = geo;
		f->m_shader = m_defaultShader;

		m_primitives.push_back(f);

		return f;

	}

	Visualizer::Sphere::Sphere() : m_radius(1.0f)
	{
	}


	//FRUSTUM ===================================================
	Visualizer::FrustumPtr Visualizer::frustum( float fov, float nearz, float farz, float aspect )
	{
		Visualizer::FrustumPtr f = std::make_shared<Visualizer::Frustum>( fov, nearz, farz, aspect );

		gfx::Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
		int numPoints = 8;
		gfx::Attribute::Ptr pAttr = geo->getAttr("P");
		pAttr->resize(numPoints);
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		geo->addLine( 0, 1 );
		geo->addLine( 1, 2 );
		geo->addLine( 2, 3 );
		geo->addLine( 3, 0 );

		geo->addLine( 4, 5 );
		geo->addLine( 5, 6 );
		geo->addLine( 6, 7 );
		geo->addLine( 7, 4 );

		geo->addLine( 0, 4 );
		geo->addLine( 1, 5 );
		geo->addLine( 2, 6 );
		geo->addLine( 3, 7 );


		f->m_geo = geo;
		f->m_shader = m_defaultShader;
		f->updatePoints();

		m_primitives.push_back(f);

		return f;
	}

	Visualizer::Frustum::Frustum( float fov, float nearz, float farz, float aspect )
	{
		m_fov = fov;
		m_nearz = nearz;
		m_farz = farz;
		m_aspect = aspect;
		m_origin = math::Vec3f(0.0f);
		m_direction = math::Vec3f(0.0f, 0.0f, -1.0f);
	}

	float Visualizer::Frustum::getFov()
	{
		return m_fov;
	}

	void Visualizer::Frustum::setFov( float fov )
	{
		m_fov = fov;
		updatePoints();
	}

	float Visualizer::Frustum::getNearZ()
	{
		return m_nearz;
	}

	void Visualizer::Frustum::setNearZ( float nearz )
	{
		m_nearz = nearz;
		updatePoints();
	}

	float Visualizer::Frustum::getFarZ()
	{
		return m_farz;
	}

	void Visualizer::Frustum::setFarZ( float farz )
	{
		m_farz = farz;
		updatePoints();
	}

	float Visualizer::Frustum::getAspect()
	{
		return m_aspect;
	}

	void Visualizer::Frustum::setAspect( float aspect )
	{
		m_aspect = aspect;
		updatePoints();
	}

	void Visualizer::Frustum::setOrigin( const math::Vec3f &origin )
	{
		m_origin = origin;
		updatePoints();
	}

	math::Vec3f Visualizer::Frustum::getOrigin()
	{
		return m_origin;
	}

	void Visualizer::Frustum::setDirection( const math::Vec3f &dir )
	{
		m_direction = dir;
		updatePoints();
	}

	math::Vec3f Visualizer::Frustum::getDirection()
	{
		return m_direction;
	}


	void Visualizer::Frustum::updatePoints()
	{
		math::Vec3f up(0.0f, 1.0f, 0.0f);
		math::Vec3f right = math::cross(m_direction,up);

		math::Vec3f center_far = m_origin + m_direction*m_farz;
		math::Vec3f center_near = m_origin + m_direction*m_nearz;

		right = math::normalize(right);
		up = math::normalize(math::cross(right, m_direction));

		// these heights and widths are half the heights and widths of the near and far plane rectangles
		float near_height = tan(m_fov/2.0f) * m_nearz;
		float near_width = near_height * m_aspect;
		float far_height = tan(m_fov/2.0f) * m_farz;
		float far_width = far_height * m_aspect;

		gfx::Attribute::Ptr pAttr = m_geo->getAttr( "P" );
		pAttr->set<math::Vec3f>( 0, center_near - up*near_height - right*near_width );
		pAttr->set<math::Vec3f>( 1, center_near - up*near_height + right*near_width );
		pAttr->set<math::Vec3f>( 2, center_near + up*near_height + right*near_width );
		pAttr->set<math::Vec3f>( 3, center_near + up*near_height - right*near_width );

		pAttr->set<math::Vec3f>( 4, center_far - up*far_height - right*far_width );
		pAttr->set<math::Vec3f>( 5, center_far - up*far_height + right*far_width );
		pAttr->set<math::Vec3f>( 6, center_far + up*far_height + right*far_width );
		pAttr->set<math::Vec3f>( 7, center_far + up*far_height - right*far_width );
	}






	// BaseVectors ====================================================================

	Visualizer::BaseVectorsPtr Visualizer::baseVectors( float scale )
	{

	//	Visualizer::BaseVectorsPtr bv = std::make_shared<Visualizer::BaseVectors>( scale );

	//	Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
	//	int numPoints = 6;
	//	Attribute::Ptr pAttr = geo->getAttr("P");
	//	pAttr->resize(numPoints);
	//	Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
	//	geo->setAttr( "Cd", cAttr );

	//	pAttr->set<math::Vec3f>(0, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
	//	pAttr->set<math::Vec3f>(1, math::Vec3f(1.0f, 0.0f, 0.0f)*scale);
	//	pAttr->set<math::Vec3f>(2, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
	//	pAttr->set<math::Vec3f>(3, math::Vec3f(0.0f, 1.0f, 0.0f)*scale);
	//	pAttr->set<math::Vec3f>(4, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
	//	pAttr->set<math::Vec3f>(5, math::Vec3f(0.0f, 0.0f, 1.0f)*scale);

	//	cAttr->set<math::Vec3f>(0, math::Vec3f(1.0f, 0.0f, 0.0f));
	//	cAttr->set<math::Vec3f>(1, math::Vec3f(1.0f, 0.0f, 0.0f));
	//	cAttr->set<math::Vec3f>(2, math::Vec3f(0.0f, 1.0f, 0.0f));
	//	cAttr->set<math::Vec3f>(3, math::Vec3f(0.0f, 1.0f, 0.0f));
	//	cAttr->set<math::Vec3f>(4, math::Vec3f(0.0f, 0.0f, 1.0f));
	//	cAttr->set<math::Vec3f>(5, math::Vec3f(0.0f, 0.0f, 1.0f));

	//	geo->addLine( 0, 1 );
	//	geo->addLine( 2, 3 );
	//	geo->addLine( 4, 5 );

	//	bv->m_geo = geo;
	//	bv->m_shader = m_defaultShader;

	//	m_primitives.push_back(bv);

	//	return bv;

		return baseVectors( math::Vec3f(1.0f, 0.0f, 0.0f), math::Vec3f(0.0f, 1.0f, 0.0f), math::Vec3f(0.0f, 0.0f, 1.0f), scale );
	}

	Visualizer::BaseVectorsPtr Visualizer::baseVectors( const math::V3f &v0, const math::V3f &v1, const math::V3f &v2, float scale )
	{
		Visualizer::BaseVectorsPtr bv = std::make_shared<Visualizer::BaseVectors>( scale );

		gfx::Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
		int numPoints = 6;
		gfx::Attribute::Ptr pAttr = geo->getAttr("P");
		pAttr->resize(numPoints);
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		pAttr->set<math::Vec3f>(0, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
		pAttr->set<math::Vec3f>(1, v0*scale);
		pAttr->set<math::Vec3f>(2, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
		pAttr->set<math::Vec3f>(3, v1*scale);
		pAttr->set<math::Vec3f>(4, math::Vec3f(0.0f, 0.0f, 0.0f)*scale);
		pAttr->set<math::Vec3f>(5, v2*scale);

		cAttr->set<math::Vec3f>(0, math::Vec3f(1.0f, 0.0f, 0.0f));
		cAttr->set<math::Vec3f>(1, math::Vec3f(1.0f, 0.0f, 0.0f));
		cAttr->set<math::Vec3f>(2, math::Vec3f(0.0f, 1.0f, 0.0f));
		cAttr->set<math::Vec3f>(3, math::Vec3f(0.0f, 1.0f, 0.0f));
		cAttr->set<math::Vec3f>(4, math::Vec3f(0.0f, 0.0f, 1.0f));
		cAttr->set<math::Vec3f>(5, math::Vec3f(0.0f, 0.0f, 1.0f));

		geo->addLine( 0, 1 );
		geo->addLine( 2, 3 );
		geo->addLine( 4, 5 );

		bv->m_geo = geo;
		bv->m_shader = m_defaultShader;

		m_primitives.push_back(bv);

		return bv;
	}

	Visualizer::BaseVectors::BaseVectors( float scale ) : Primitive(), m_scale(scale)
	{
	}

	void Visualizer::BaseVectors::setScale( const float scale )
	{
		m_scale = scale;
	}

	float Visualizer::BaseVectors::getScale()const
	{
		return m_scale;
	}



	// GRID =====================================================

	Visualizer::GridPtr Visualizer::grid( int xres, int zres)
	{
		Visualizer::GridPtr g = std::make_shared<Visualizer::Grid>();

		gfx::Geometry::Ptr geo = gfx::geo_grid( xres, zres, gfx::Geometry::LINE );
		int numPoints = geo->getAttr("P")->numElements();
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(0.6f, 0.6f, 0.6f));

		g->m_geo = geo;
		g->m_shader = m_defaultShader;

		m_primitives.push_back(g);

		return g;
	}


	Visualizer::Grid::Grid()
	{

	}


	// CAMERA ========================================================

	void Visualizer::camera( gfx::CameraPtr cam )
	{
		Visualizer::CameraPtr c = std::make_shared<Visualizer::Camera>( cam );





		// Frustum geometry -----------------
		gfx::Geometry::Ptr geo = gfx::Geometry::createLineGeometry();
		int numPoints = 8;
		gfx::Attribute::Ptr pAttr = geo->getAttr("P");
		pAttr->resize(numPoints);
		gfx::Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
		geo->setAttr( "Cd", cAttr );

		for( int i=0;i<numPoints;++i )
			cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));

		geo->addLine( 0, 1 );
		geo->addLine( 1, 2 );
		geo->addLine( 2, 3 );
		geo->addLine( 3, 0 );

		geo->addLine( 4, 5 );
		geo->addLine( 5, 6 );
		geo->addLine( 6, 7 );
		geo->addLine( 7, 4 );

		geo->addLine( 0, 4 );
		geo->addLine( 1, 5 );
		geo->addLine( 2, 6 );
		geo->addLine( 3, 7 );


		c->m_geo = geo;
		c->m_shader = m_defaultShader;
		c->updatePoints();

		c->setTransform( cam->m_transform );

		m_primitives.push_back(c);
	}


	Visualizer::Camera::Camera( gfx::CameraPtr cam ) : Frustum( cam->m_fov, cam->m_znear, cam->m_zfar, cam->m_aspectRatio), m_cam(cam)
	{
	}

	void Visualizer::Camera::preRender()
	{
		// update camera info
		setTransform(m_cam->m_transform);
	}


	// VectorField ===============================================

	//Visualizer::VectorFieldPtr Visualizer::field( Field3f::Ptr vectorField, const std::string &id )
	//{
	//	Visualizer::VectorFieldPtr vf = std::make_shared<Visualizer::VectorField>( vectorField );

	//	// bounds geometry -----------------
	//	Geometry::Ptr geo = gfx::geo_cube( math::Box3f(math::Vec3f(0.0f), math::Vec3f(1.0f)), gfx::Geometry::LINE );
	//	Attribute::Ptr pAttr = geo->getAttr("P");
	//	int numPoints = pAttr->numElements();
	//	Attribute::Ptr cAttr = gfx::Attribute::createVec3f(numPoints);
	//	geo->setAttr( "Cd", cAttr );

	//	for( int i=0;i<numPoints;++i )
	//		cAttr->set<math::Vec3f>(i, math::Vec3f(1.0f, 1.0f, 1.0f));


	//	vf->m_geo = geo;
	//	vf->m_shader = m_defaultShader;
	//	vf->update();

	//	m_primitives.push_back(vf);

	//	if( !id.empty() )
	//		m_primitiveIds[id] = vf;

	//	return vf;
	//}

	//Visualizer::VectorFieldPtr Visualizer::field( const std::string &id )
	//{
	//	PrimitivePtr prim = getPrimitive( id );
	//	if( prim && std::dynamic_pointer_cast<VectorField>(prim) )
	//		return std::dynamic_pointer_cast<VectorField>(prim);
	//	return Visualizer::VectorFieldPtr();
	//}


	//Visualizer::VectorField::VectorField( Field3f::Ptr field ) : Primitive(), m_field(field), m_resolution(-1, -1, -1), m_showCenters(false), m_vectorScale(1.0f), m_vectorLengthCapEnabled(false), m_vectorLengthCapSqr(1.0f), m_vectorLengthCap(1.0f)
	//{
	//	m_vectors = Geometry::createLineGeometry();
	//	m_vectors->setAttr( "Cd", Attribute::createVec3f() );

	//	m_centers = Geometry::createPointGeometry();
	//	m_centers->setAttr( "Cd", Attribute::createVec3f() );

	//	m_mode = 0; // vectors
	//}

	//void Visualizer::VectorField::setMode( int mode )
	//{
	//	m_mode = mode;
	//	update();
	//}

	//void Visualizer::VectorField::setShowCenters( bool showCenters)
	//{
	//	m_showCenters = showCenters;
	//}

	//void Visualizer::VectorField::setVectorScale( float scale )
	//{
	//	m_vectorScale = scale;
	//	update();
	//}

	//void Visualizer::VectorField::setVectorLengthCap( bool enabled, float max )
	//{
	//	m_vectorLengthCapEnabled = enabled;
	//	m_vectorLengthCap = max;
	//	m_vectorLengthCapSqr = max*max;
	//	update();
	//}

	//void Visualizer::VectorField::update()
	//{
	//	math::V3i fieldRes = m_field->getResolution();
	//	if( (m_resolution.x != fieldRes.x)||
	//		(m_resolution.y != fieldRes.y)||
	//		(m_resolution.z != fieldRes.z))
	//	{
	//		// update bounding box
	//		{
	//			Attribute::Ptr pAttr = m_geo->getAttr("P");
	//			pAttr->set<math::V3f>( 0, m_field->localToWorld(math::V3f(0, 0, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 1, m_field->localToWorld(math::V3f(0, 1.0f, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 2, m_field->localToWorld(math::V3f(1.0f, 1.0f, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 3, m_field->localToWorld(math::V3f(1.0f, 0, 1.0f ) ) );

	//			pAttr->set<math::V3f>( 4, m_field->localToWorld(math::V3f(0, 0, 0 ) ) );
	//			pAttr->set<math::V3f>( 5, m_field->localToWorld(math::V3f(0, 1.0f, 0 ) ) );
	//			pAttr->set<math::V3f>( 6, m_field->localToWorld(math::V3f(1.0f, 1.0f, 0 ) ) );
	//			pAttr->set<math::V3f>( 7, m_field->localToWorld(math::V3f(1.0f, 0, 0 ) ) );
	//		}

	//		m_resolution = fieldRes;
	//	}

	//	// resize voxel visualization geometry
	//	int numVoxels = fieldRes.x*fieldRes.y*fieldRes.z;
	//	m_vectors->clear();
	//	m_centers->clear();
	//	//m_mode = 0;
	//	switch(m_mode)
	//	{
	//	case 1:
	//		{
	//			gfx::Attribute::Ptr pAttr = m_vectors->getAttr("P");
	//			gfx::Attribute::Ptr cAttr = m_vectors->getAttr("Cd");

	//			std::vector<math::V3f> streakLineStartPoints;


	//			int numStreakLines = (int)streakLineStartPoints.size();
	//			for( int i=0;i<numStreakLines;++i )
	//			{
	//				// generate streak line
	//				math::V3f p1 = streakLineStartPoints[i];
	//				int numSubDivisions = 100;
	//				float stepSize = 0.01f;
	//				for( int j=0;j<numSubDivisions;++j )
	//				{
	//					if( !m_field->bound().encloses(p1) )
	//						break;
	//					math::V3f vel = m_field->evaluate( m_field->worldToVoxel(p1) ).normalized();
	//					math::V3f p2 = p1+vel*stepSize;
	//					int idx = pAttr->appendElement<math::V3f>(p1);
	//					pAttr->appendElement<math::V3f>(p2);
	//					cAttr->appendElement<math::V3f>(math::V3f(0.9f, 0.5f, 0.5f));
	//					cAttr->appendElement<math::V3f>(math::V3f(0.9f, 0.5f, 0.5f));
	//					m_vectors->addLine( idx, idx+1 );
	//					p1 = p2;
	//				}
	//			}
	//		}break; // 1, streak lines
	//	case 0:
	//	default:
	//		// udate voxelvalues
	//		{
	//			Attribute::Ptr vectors_pAttr = m_vectors->getAttr("P");
	//			Attribute::Ptr vectors_cAttr = m_vectors->getAttr("Cd");
	//			Attribute::Ptr centers_pAttr = m_centers->getAttr("P");
	//			Attribute::Ptr centers_cAttr = m_centers->getAttr("Cd");
	//			const math::V3f *ptr = m_field->getRawPointer();
	//			for( int k=0;k<fieldRes.z;++k )
	//			{
	//				//if( k % 2 != 0 )
	//				//	continue;
	//				for( int j=0;j<fieldRes.y;++j )
	//				{
	//					//if( j % 2 != 0 )
	//					//	continue;

	//					for( int i=0;i<fieldRes.x;++i, ++ptr )
	//					{
	//						//if( i % 2 != 0 )
	//						//continue;


	//						math::V3f vsP( i+0.5f, j+0.5f, k+0.5f );
	//						math::V3f wsP = m_field->voxelToWorld(vsP);
	//						int idx = vectors_pAttr->appendElement<math::V3f>( wsP );

	//						math::V3f vec = *ptr*m_vectorScale;

	//						if( m_vectorLengthCapEnabled )
	//						{
	//							float l = vec.getSquaredLength();
	//							if( l>m_vectorLengthCapSqr )
	//								vec = vec.normalized()*std::min( l, m_vectorLengthCap );
	//						}

	//						vectors_pAttr->appendElement<math::V3f>( wsP + vec);
	//						vectors_cAttr->appendElement<math::V3f>( math::V3f(1.0f) );
	//						vectors_cAttr->appendElement<math::V3f>( math::V3f(1.0f) );
	//						m_vectors->addLine( idx, idx+1 );

	//						idx = centers_pAttr->appendElement<math::V3f>( wsP );
	//						centers_cAttr->appendElement<math::V3f>( math::V3f(1.0f) );
	//						m_centers->addPoint( idx );
	//					}
	//				}
	//			} // for...
	//		}break; // default, 0, vector disply
	//	}; // switch mode
	//}

	//void Visualizer::VectorField::preRender()
	//{
	//}


	//void Visualizer::VectorField::render( ContextPtr context )
	//{
	//	context->render( m_geo, m_shader );

	//	context->render( m_vectors, m_shader );

	//	if( m_showCenters )
	//	{
	//		glPointSize(4.0f);
	//		context->render( m_centers, m_shader );
	//	}
	//}

	//// ScalarField =======================================
	//Visualizer::ScalarField::ScalarField( Fieldf::Ptr field ) : Primitive(), m_field(field), m_resolution(-1, -1, -1)
	//{
	//	m_centers = Geometry::createPointGeometry();
	//	m_centers->setAttr( "Cd", Attribute::createVec3f() );
	//}

	//void Visualizer::ScalarField::update()
	//{
	//	math::V3i fieldRes = m_field->getResolution();
	//	int numVoxels = fieldRes.x*fieldRes.y*fieldRes.z;
	//	if( (m_resolution.x != fieldRes.x)||
	//		(m_resolution.y != fieldRes.y)||
	//		(m_resolution.z != fieldRes.z))
	//	{
	//		// update bounding box
	//		{
	//			Attribute::Ptr pAttr = m_geo->getAttr("P");
	//			pAttr->set<math::V3f>( 0, m_field->localToWorld(math::V3f(0, 0, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 1, m_field->localToWorld(math::V3f(0, 1.0f, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 2, m_field->localToWorld(math::V3f(1.0f, 1.0f, 1.0f ) ) );
	//			pAttr->set<math::V3f>( 3, m_field->localToWorld(math::V3f(1.0f, 0, 1.0f ) ) );

	//			pAttr->set<math::V3f>( 4, m_field->localToWorld(math::V3f(0, 0, 0 ) ) );
	//			pAttr->set<math::V3f>( 5, m_field->localToWorld(math::V3f(0, 1.0f, 0 ) ) );
	//			pAttr->set<math::V3f>( 6, m_field->localToWorld(math::V3f(1.0f, 1.0f, 0 ) ) );
	//			pAttr->set<math::V3f>( 7, m_field->localToWorld(math::V3f(1.0f, 0, 0 ) ) );
	//		}

	//		// resize voxel visualization geometry
	//		m_centers->clear();

	//		m_resolution = fieldRes;
	//	}

	//	// udate voxelvalues
	//	{
	//		Attribute::Ptr centers_pAttr = m_centers->getAttr("P");
	//		Attribute::Ptr centers_cAttr = m_centers->getAttr("Cd");
	//		const float *ptr = m_field->getRawPointer();
	//		for( int k=0;k<fieldRes.z;++k )
	//			for( int j=0;j<fieldRes.y;++j )
	//				for( int i=0;i<fieldRes.x;++i, ++ptr )
	//				{
	//					math::V3f vsP( i+0.5f, j+0.5f, k+0.5f );
	//					math::V3f wsP = m_field->voxelToWorld(vsP);

	//					int idx = centers_pAttr->appendElement<math::V3f>( wsP );
	//					centers_cAttr->appendElement<math::V3f>( math::V3f(1.0f) );
	//					m_centers->addPoint( idx );
	//				}

	//	}
	//}

	//void Visualizer::ScalarField::preRender()
	//{
	//}


	//void Visualizer::ScalarField::render( ContextPtr context )
	//{
	//	context->render( m_geo, m_shader );

	//	glPointSize(4.0f);
	//	context->render( m_centers, m_shader );
	//}




	*/
}
