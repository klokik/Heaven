#include <iostream>

#include "AEDebug.h"
#include "AEResourceManager.h"

#include "StaticMeshLibrary.hpp"


namespace heaven
{
	std::map<std::string,AEMesh*> StaticMeshLibrary::mesh_library;

	AEMesh *StaticMeshLibrary::getMesh(std::string name)
	{
		try
		{
			return mesh_library.at(name);
		}
		catch(const std::out_of_range &oor)
		{
			return nullptr;
		}
	}

	void StaticMeshLibrary::loadMesh(std::string name,std::string filename)
	{
		if(mesh_library.find(name)!=mesh_library.end())
			return;

		aengine::AEPrintLog("Load file: "+filename);

		AEMesh *mesh=new AEMesh;

		aengine::AEResourceManager::LoadMesh(filename,*mesh);
		mesh_library.insert(std::pair<std::string,AEMesh*>(name,mesh));
	}
}