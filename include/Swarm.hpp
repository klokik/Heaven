#ifndef SWARM_H_
#define SWARM_H_

#include <vector>

#include "AEObject.h"
#include "AEVectorMath.h"

namespace heaven
{
	class SwarmItem: public virtual aengine::AEObject
	{
	public:
		float speed = 1.0f;
		Vec3f direction = vec3f(1.0f,0.0f,0.0f);

	//	float radius_repulsion;
		float radius_alingment = 3.0f;
	//	float radius_attraction;
		float radius_gain = 1.0f;
		float fov = 90.0f;

		AEObject *attractor = nullptr;	// FIXME: weak_ptr????

		bool fixed = false;

		bool gain(void)
		{
			if(!attractor)
				return false;

			auto chase_dir = attractor->GetAbsPosition() - this->GetAbsPosition();

			return (SqrLength(chase_dir) < radius_gain*radius_gain);
		}
	};

	template<class T,class Container>
	class SwarmSystem
	{
	public:
		static void swarm_one(T const &item_ptr,Container &items_ptr,float dt_ms)
		{
			Vec3f prev_dir = item_ptr->direction;
			Vec3f desired_dir = vec3f(0,0,0);

			if(item_ptr->attractor)
				desired_dir = item_ptr->attractor->GetAbsPosition() - item_ptr->GetAbsPosition();
			else
				return; // FIXME: generally we need to continue move in the specific direction, just debugging...

			item_ptr->direction = normalize(prev_dir*0.9f+normalize(desired_dir)*0.1f);

			for(auto it_ptr:items_ptr)
			{
				auto chase_dir = it_ptr->GetAbsPosition()-item_ptr->GetAbsPosition();
				if(*it_ptr != *item_ptr && SqrLength(chase_dir) < item_ptr->radius_alingment*item_ptr->radius_alingment)
				{
					item_ptr->direction = (item_ptr->direction + flip(chase_dir,item_ptr->direction))*0.5f;
				}
			}

			if(SqrLength(item_ptr->direction)!=0)
				item_ptr->SetTranslate(item_ptr->translate + normalize(item_ptr->direction)*item_ptr->speed*(dt_ms/1000));
		}

		static void swarm(Container &items_ptr,float dt_ms)
		{
			for(auto item_ptr:items_ptr)
			{
				swarm_one(item_ptr,items_ptr,dt_ms);
			}
		}
	};
}

#endif /* SWARM_H_ */