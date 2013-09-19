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
		bool empty;
		bool connection_opened;
		Side **thread_owner;

		static void *listen(void *param);
		void init(void);
	public:
		uint32_t uid;
		std::wstring name;
		AEColor color;
		bool done;

		std::map<std::string,float> resources;

		Side(void);
		Side(std::wstring name,uint32_t server);
		Side(Side &&s);

		Side &operator=(Side &&s);

		int connect(uint32_t server);
		void disconnect(void);

		void send(HPacket cmd);
		void handlePacket(HPacket cmd);

		void update(float dt_ms);

		~Side(void);
	};
}
#endif /* SIDE_H_ */