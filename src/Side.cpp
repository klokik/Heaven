#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>

#include "AEVectorMath.h"

#include "Side.hpp"
#include "Server.hpp"
#include "HeavenWorld.hpp"

namespace heaven
{
	Side::Side(void)
	{
		socket = -1;
	}

	Side::Side(std::string name,uint32_t server)
	{
		Side();
		this->name = name;
		color.vec = vec4f(1.0f,1.0f,1.0f,1.0f);
	}

	int Side::connect(uint32_t server)
	{
		int port = 1234;

		this->socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		assert(socket!=-1);

		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr.s_addr);
		server_addr.sin_port = htons(port);

		int status = ::connect(socket,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr));
		assert(status==0);

		done = false;
		pthread_create(&thread,nullptr,listen,static_cast<void*>(this));

		return 1;
	}

	void Side::disconnect(void)
	{
		done = true;
		if(socket!=-1)
		{
			int status = shutdown(socket,2);
			switch(status)
			{
			case 0:
				std::cout<<"socket closed successfully"<<std::endl;
				break;
			case EBADF:
				std::cout<<"socket is not a valid file descriptor."<<std::endl;
				break;
			case ENOTSOCK:
				std::cout<<"socket is not a socket."<<std::endl;
				break;
			case ENOTCONN:
				std::cout<<"socket is not connected"<<std::endl;
				break;
			}

			socket = -1;
		}
	}

	void Side::send(HPacket cmd)
	{
		assert(socket!=-1);

		write(socket,reinterpret_cast<char*>(&cmd),sizeof(uint32_t)*2);
		write(socket,cmd.data,sizeof(char)*cmd.data_size);
	}

	void *Side::listen(void *param)
	{
		Side *local_this = static_cast<Side*>(param);

		while(!local_this->done)
		{
			try
			{
				HPacket cmd = Server::readPacket(local_this->socket);
				local_this->handlePacket(cmd);
				cmd.free();
			}
			catch(int i)
			{
				std::cout<<"server read error: "<<i<<std::endl;
				break;
			}
		}
		local_this->disconnect();

		return nullptr;
	}

	void Side::handlePacket(HPacket cmd)
	{
		uint32_t *data = reinterpret_cast<uint32_t *>(cmd.data);

		switch(cmd.msg_type)
		{
		case 0x72: //transfer ship
			HeavenWorld::instance->transfer(data[0],data[1],static_cast<float>(data[3]));
			break;
		case 0xd2: //delete ship
			HeavenWorld::instance->destroyWarship(data[0]);
			break;
		case 0xa2: //add/alter ship
			HeavenWorld::instance->addWarship(new Ship("glider",1));//cmd));
			break;
		case 0xa1: //add island
			break;
		case 0x41: //update island
			break;
		default:
			std::cout<<"packet accepted, data_size = "<<cmd.data_size<<std::endl;
		}
	}

	Side::~Side(void)
	{
		disconnect();
		pthread_join(thread,nullptr);
	}
}