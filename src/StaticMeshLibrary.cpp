#include "StaticMeshLibrary.hpp"


void LoadObjFile(AEMesh *&mesh, const char *path);

namespace heaven
{
	using namespace std;


	map<string,AEMesh*> StaticMeshLibrary::mesh_library;

	AEMesh *StaticMeshLibrary::getMesh(string name)
	{
		try
		{
			return mesh_library.at(name);
		}
		catch(const out_of_range &oor)
		{
			return nullptr;
		}
	}

	void StaticMeshLibrary::loadMesh(std::string name,std::string filename)
	{
		if(mesh_library.find(name)!=mesh_library.end())
			return;

		AEMesh *mesh;

		LoadObjFile(mesh,filename.c_str());
		mesh_library.insert(pair<string,AEMesh*>(name,mesh));
	}
}