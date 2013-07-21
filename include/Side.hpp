#ifndef SIDE_H_
#define SIDE_H_

#include <cstdint>
#include <string>
#include <map>

#include "AEUnits.h"

namespace heaven
{
	class HeavenWorld;

	class Side
	{
	protected:

	public:
		uint32_t uid;
		std::string name;
		AEColor color;

		std::map<std::string,float> resources;

		Side(void);
		Side(std::string name,uint32_t server);

		int connect(uint32_t server);
		void disconnect(void);

		void Sync(HeavenWorld *world);

		~Side(void);
	};
}
#endif /* SIDE_H_ */