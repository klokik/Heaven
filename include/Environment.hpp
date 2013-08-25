#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "AEObjectMesh.h"
#include "AEObjectLight.h"
#include "AEObjectEmpty.h"
#include "AEResourceManager.h"
#include "AEVectorMath.h"


namespace heaven
{
	using namespace aengine;

	class Environment: public AEObjectEmpty
	{
	private:
		AEObjectMesh *skybox;
		AEObjectMesh *ground;
		AEObjectLight *sunlight;
		AEObjectLight *ambient;

		Vec4f getSkyColor(float day_time)
		{
			//float pi = 3.1415926f;
			//float x = day_time;
			Vec3f sky_blue = vec3f(0.53f,0.81f,0.92f);	// According to WolfrAmalpha)
			float darkness = 1.0f;//0.5f*(1+sin((x-pi/2)*pi/12));

			return vec4f(sky_blue*darkness,1.0f);
		}
	public:
		Environment(void)
		{
			this->name = "environment";

			skybox = new AEObjectMesh; skybox->name = "skybox";
			ground = new AEObjectMesh; ground->name = "ground";
			skybox->mesh = new AEMesh;
			ground->mesh = new AEMesh;
			AEResourceManager::LoadMesh("res/models/sky.obj",*skybox->mesh);
			AEResourceManager::LoadMesh("res/models/ground.obj",*ground->mesh);

			sunlight = new AEObjectLight(AE_DIRECTIONAL_LIGHT);
			sunlight->name = "sunlight";

			ambient = new AEObjectLight(AE_AMBIENT_LIGHT);
			ambient->name = "ambient";
			ambient->color.vec = vec4f(1.0f,1.0f,1.0f,0.2f);

			this->AddChild(sunlight);
			this->AddChild(ambient);
			this->AddChild(skybox);
			this->AddChild(ground);
		}

		void setMaterials(AEMaterialManager &manag)
		{
			skybox->material = manag.New();
			ground->material = manag.New();

			skybox->material->shading=false;
			ground->material->shading=true;
			skybox->material->diffuse.vec=vec4f(0.53f,0.81f,0.92f,1.0f);
			ground->material->diffuse.vec=vec4f(1.0f,1.0f,1.0f,1.0f);

			// ground->material->texture = new AEImage;
			// AEResourceManager::LoadImage("res/img/ground.png",*ground->material->texture,"png");
		}

		void update(float game_time)
		{
			if(!skybox->material) return;

			skybox->material->diffuse.vec = getSkyColor(game_time);

			sunlight->SetRotate(vec3f(360*game_time/24,0,0));
		}
	};
}

#endif /* ENVIRONMENT_H_ */