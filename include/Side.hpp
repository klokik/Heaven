#ifndef SIDE_H_
#define SIDE_H_

#include <cstdint>
#include <string>
#include <map>

#include "AEUnits.h"

#include "HPacket.hpp"

namespace heaven
{
	class HeavenWorld;

	class Side
	{
	protected:
		int socket;
		pthread_t thread;

		static void *listen(void *param);
	public:
		uint32_t uid;
		std::string name;
		AEColor color;
		bool done;

		std::map<std::string,float> resources;

		Side(void);
		Side(std::string name,uint32_t server);

		int connect(uint32_t server);
		void disconnect(void);

		void send(HPacket cmd);
		void handlePacket(HPacket cmd);

		~Side(void);
	};
}
#endif /* SIDE_H_ */