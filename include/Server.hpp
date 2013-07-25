#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>
#include <map>
#include <vector>
#include <string>

#include "Side.hpp"
#include "HPacket.hpp"


namespace heaven
{
	class Server
	{
	protected:
		class Client
		{
		private:
			int socket;
			pthread_t thread;
			Client **thread_owner;
			Server *server;
			bool done;
			bool empty;

			static void *listen(void *param);

		public:
			Client(int socket,Server *srv);
			Client(Client &&c);

			Client &operator=(Client &&c);

			int getUid(void);
			void start(void);
			void send(HPacket cmd);
			int ping(void);
			void disconnect(void);
			void kill(std::string msg);
			void handlePacket(HPacket cmd);

			//Client &operator=(Client &a);

			~Client(void);
		};

		// std::map<uint32_t,Client> clients;
		std::vector<Client> clients;
		pthread_t listen_thread;
		bool done;

		int listen_socket;
		uint16_t port;

		static void *listenClients(void *param);
		void acceptClient(int socket);

		void processMsg(HPacket cmd);
		void sendAll(HPacket cmd);

		// static void *clientThreadLoop(void *param);

	public:
		Server(HeavenWorld *world);

		static HPacket readPacket(int socket);

		int start();
		void stop();

		~Server(void);
	};
}

#endif /* SERVER_H_ */