#ifndef CAMERATARGET_H_
#define CAMERATARGET_H_

#include "AEObjectEmpty.h"
#include "AEObjectCamera.h"

namespace heaven
{
	class CameraTarget: public AEObjectEmpty
	{
	public:
		CameraTarget(AEObjectCamera *camera);

		void setDistance(float distance);
		void alterDistance(float delta);

		void setPitch(float angle);
		void setYaw(float angle);

		void relPitch(float angle_delta);
		void relYaw(float angle_delta);

		~CameraTarget(void);
	};
}

#endif /* CAMERATARGET_H_ */