#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <random>
#include <functional>
#include <cstdlib>

#include "AEObjectMesh.h"
#include "AEObjectLight.h"
#include "AEObjectEmpty.h"
#include "AEObjectSprite.h"
#include "AEResourceManager.h"
#include "AEVectorMath.h"
#include "AEDebug.h"

#define cloud_num 100

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

		AEObjectSprite *clouds[cloud_num];

		Vec4f getSkyColor(float day_time)
		{
			//float pi = 3.1415926f;
			//float x = day_time;
			Vec3f sky_blue = vec3f(0.53f,0.81f,0.92f);	// According to WolfrAmalpha)
			float darkness = 1.0f;//0.5f*(1+sin((x-pi/2)*pi/12));

			return vec4f(sky_blue*darkness,1.0f);
		}

		void addClouds(AEMaterialManager &manag)
		{
			std::default_random_engine generator;
			std::uniform_int_distribution<int> distribution(-300,300);
			auto prg = std::bind(distribution,generator);

			AEMaterial *cloud_mat = manag.New();
			cloud_mat->shading = false;
			cloud_mat->diffuse.vec = vec4f(1.0f,1.0f,1.0f,1.0f);
			cloud_mat->transparent = true;

			AEImage *cloud_tex[3];
			char file_names[][20] = {
				"res/img/cloud_1.png",
				"res/img/cloud_2.png",
				"res/img/cloud_3.png"
			};

			int result = AE_OK;
			for(int q=0;q<3;q++)
			{
				cloud_tex[q] = new AEImage;
				if(!AEResourceManager::LoadImage(file_names[q],*cloud_tex[q],"png"))
				{
					delete cloud_tex[q];
					cloud_tex[q] = nullptr;
					result&=AE_ERR;
				}
			}

			if(result)
				AEPrintLog("Clouds load ok");

			for(int q=0;q<cloud_num;q++)
			{
				AEObjectSprite *cloud = new AEObjectSprite;
				clouds[q] = cloud;
				cloud->name = std::string("cloud_")+(char)('a'+q);
				this->AddChild(cloud);

				cloud->projection = AE_PERSPECTIVE;
				cloud->material = manag.New();
				*cloud->material = *cloud_mat;
				cloud->material->texture = cloud_tex[q%3];

				cloud->SetScale(vec3f(5.0f,5.0f,1.0f));

				Vec3f translate = vec3f(prg()*0.1f,-4.0f-(prg()%2),prg()*0.1f);
				cloud->SetTranslate(translate);
				float scale = abs(prg())/30.0f+5.0f;
				cloud->SetScale(vec3f(scale,fabs(scale),scale));
			}
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

			ground->SetTranslate(vec3f(0.0f,-40.0f,0.0f));
			ground->SetScale(vec3f(10.0f,10.0f,10.0f));
			skybox->SetScale(vec3f(2.0f,2.0f,2.0f));

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

			ground->material->texture = new AEImage;
			if(!AEResourceManager::LoadImage("res/img/ground.png",*ground->material->texture,"png"))
			{
				delete ground->material->texture;
				ground->material->texture=nullptr;
			}
			else
			{
				dbgout()<<"Ground: "<<
					ground->material->texture->width<<"x"<<
					ground->material->texture->height<<"x"<<
					ground->material->texture->bpp<<" - "<<
					ground->material->texture->size<<";"<<std::endl;
			}

			addClouds(manag);
		}

		void update(float game_time,float dt_ms)
		{
			if(!skybox->material) return;

			skybox->material->diffuse.vec = getSkyColor(game_time);

			sunlight->SetRotate(vec3f(360*game_time/24,0,0));

			const float cloud_radius=30;
			Vec3f wind = normalize(vec3f(0.2f,0.0f,0.5f));
			for(size_t q=0;q<cloud_num;q++)
			{
				AEObjectSprite *cloud = clouds[q];
				if(Length(vec2f(cloud->translate.X,cloud->translate.Z))>cloud_radius)
				{
					Vec3f translate = normalize(cloud->translate)*cloud_radius*(-1.0f);
					translate.Y *= -1;
					cloud->SetTranslate(translate);
				}

				cloud->RelTranslate(wind*dt_ms*0.001f);
			}
		}
	};
}

#endif /* ENVIRONMENT_H_ */