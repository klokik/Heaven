#include "AEVectorMath.h"

#include "Side.hpp"

namespace heaven
{
	Side::Side(void)
	{

	}

	Side::Side(std::string name,uint32_t server)
	{
		this->name = name;
		color.vec = vec4f(1.0f,1.0f,1.0f,1.0f);
	}

	int Side::connect(uint32_t server)
	{
		//TODO open new connection && sync
	}

	void Side::disconnect(void)
	{
		//TODO close current connection 
	}

	Side::~Side(void)
	{
		
	}
}