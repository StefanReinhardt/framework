#pragma once

#include <map>

#include <QJsonObject>

#include <core/GraphNode.h>
#include <plugins/primitives/Camera.h>








class HouScene
{
public:
	typedef std::shared_ptr<HouScene> Ptr;
	HouScene();
	~HouScene();


	Camera::Ptr                         getCamera( const QString &name );



	void                                load( const QString &filename );
private:
	Camera::Ptr                         loadCamera( QJsonObject &obj );
	std::map<QString, Camera::Ptr>      m_cameras;
};
