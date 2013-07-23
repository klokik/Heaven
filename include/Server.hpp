#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>
#include <map>

#include "Side.hpp"
#include "HPacket.hpp"


namespace heaven
{
	class Server
	{
	protected:
		std::map<uint32_t,Side> clients;
		pthread_t listen_thread;
		bool done;

		int listen_socket;

		static void *listenClients(void *param);
		void acceptClient(int socket);

		void processMsg(HPacket cmd);
		void sendAll(HPacket cmd);

		static HPacket readPacket(int socket);

		static void *clientThreadLoop(void *param);

	public:
		Server(void);

		int start();
		void stop();

		~Server(void);
	};
}

#endif /* SERVER_H_ */