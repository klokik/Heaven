#ifndef SERVER_H_
#define SERVER_H_

#include <map>

#include "SyncCommand.hpp"
#include "Side.hpp"


namespace heaven
{
	class Server
	{
	protected:
		std::map<uint32_t,Side> clients;

		void listenClients(void);
		void acceptClient();

		void receiveAndRespond(SyncCommand cmd);

	public:
		Server(void);

		int start();
		void stop();

		~Server(void);
	};
}

#endif /* SERVER_H_ */