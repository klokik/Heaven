#ifndef STATICMESHLIBRARY_H
#define STATICMESHLIBRARY_H

#include <map>

#include "AEMesh.h"

namespace heaven
{
	class StaticMeshLibrary
	{
	protected:
		static std::map<std::string,AEMesh*> mesh_library;
		static AEMesh *getMesh(std::string name);
		static void loadMesh(std::string name,std::string filename);
	};
}

#endif