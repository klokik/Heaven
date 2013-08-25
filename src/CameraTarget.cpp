#include "CameraTarget.hpp"

#include "AEVectorMath.h"

namespace heaven
{
	CameraTarget::CameraTarget(AEObjectCamera *camera)
	{
		this->AddChild(camera);
		this->name = "CameraTarget";
	}

	void CameraTarget::setDistance(float distance)
	{
		children[0]->SetTranslate(vec3f(0.0f,0.0f,distance));
	}

	void CameraTarget::relDistance(float delta)
	{
		children[0]->RelTranslate(vec3f(0.0f,0.0f,delta));
	}

	void CameraTarget::setPitch(float angle)
	{
		SetRotate(vec3f(angle,rotate.Y,rotate.Z));
	}

	void CameraTarget::setYaw(float angle)
	{
		SetRotate(vec3f(rotate.X,angle,rotate.Z));
	}

	void CameraTarget::relPitch(float angle_delta)
	{
		RelRotate(vec3f(angle_delta,0.0f,0.0f));
	}

	void CameraTarget::relYaw(float angle_delta)
	{
		RelRotate(vec3f(0.0f,angle_delta,0.0f));
	}

	CameraTarget::~CameraTarget(void)
	{
		while(!children.empty())
			RemoveChild(children[0]);
	}
}