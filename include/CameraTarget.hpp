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
		void relDistance(float delta);

		void setPitch(float angle);
		void setYaw(float angle);

		void relPitch(float angle_delta);
		void relYaw(float angle_delta);

		float getDistance(void);
		float getPitch(void);
		float getYaw(void);

		~CameraTarget(void);
	};
}

#endif /* CAMERATARGET_H_ */