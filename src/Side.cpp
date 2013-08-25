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
		init();
	}

	Side::Side(std::string name,uint32_t server): Side()
	{
		std::cout<<"new: "<<name<<std::endl;
		this->name = name;
	}

	Side::Side(Side &&s)
	{
		operator=(std::move(s));
	}

	void Side::init(void)
	{
		socket = -1;
		uid = 0;
		empty = false;
		color.vec = vec4f(1.0f,1.0f,1.0f,1.0f);
		thread_owner = new Side*;
		std::cout<<thread_owner<<std::endl;
		*thread_owner = this;
		connection_opened = false;
	}

	Side &Side::operator=(Side &&s)
	{
		uid = s.uid;
		socket = s.socket;
		thread = s.thread;
		name = s.name;
		color = s.color;
		thread_owner = s.thread_owner;
		*thread_owner = this;
		connection_opened = s.connection_opened;

		empty = s.empty;
		s.empty = true;

		return *this;
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
		if(status!=0)
			perror("connect");
		assert(status==0);

		done = false;
		connection_opened = true;
		pthread_create(&thread,nullptr,listen,static_cast<void*>(thread_owner));

		return 1;
	}

	void Side::disconnect(void)
	{
		done = true;
		connection_opened = false;
		if(socket!=-1)
		{
			int status = shutdown(socket,2);
			switch(status)
			{
			case 0:
				std::cout<<"socket closed successfully"<<std::endl;
				break;
			// case EBADF:
			// 	std::cout<<"socket is not a valid file descriptor."<<std::endl;
			// 	break;
			// case ENOTSOCK:
			// 	std::cout<<"socket is not a socket."<<std::endl;
			// 	break;
			// case ENOTCONN:
			// 	std::cout<<"socket is not connected"<<std::endl;
			// 	break;
			default:
				std::cout<<"socket was NOT closed successfully"<<std::endl;
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
		Side *local_this = *static_cast<Side**>(param);

		while(!local_this->done)
		{
			try
			{
				HPacket cmd = Server::readPacket(local_this->socket);
				local_this->handlePacket(cmd);
				cmd.free();
				// In case it've been changed
				local_this = const_cast<Side*>(*static_cast<Side**>(param));
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
		if(!empty&&connection_opened)
		{
			std::cout<<"destroy: "<<name<<std::endl;
			done = true;
			disconnect();
			std::cout<<"joining to listener process: ... ";
			pthread_join(thread,nullptr);
			std::cout<<"done: "<<thread_owner<<std::endl;
			delete thread_owner;
			empty = true;
		}
	}
}