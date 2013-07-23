#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#include "Server.hpp"

namespace heaven
{
	Server::Server(void)
	{

	}

	void *Server::listenClients(void *param)
	{
		Server *local_this = reinterpret_cast<Server*>(param);
		int listen_socket = *reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(param)+sizeof(Server*));

		int status = listen(listen_socket, 5);

		if(status==-1)
		{
			std::cout<<"Failed to open socket for listening"<<std::endl;
			throw 9;
		}

		std::cout<<"OK, listening client on port 1234"<<std::endl;

		local_this->done = false;
		while(!local_this->done)
		{
			local_this->acceptClient(listen_socket);
		}
		
		return nullptr;
	}

	void Server::acceptClient(int socket)
	{
		int client_socket = accept(socket,nullptr,nullptr);

		if(client_socket==-1)
		{
			std::cout<<"Failed to accept client"<<std::endl;
			return;
		}

		std::cout<<"Client accepted"<<std::endl;

		pthread_create(&listen_thread,nullptr,clientThreadLoop,static_cast<void*>(new std::pair<Server*,int>(this,client_socket)));
	}

	void Server::processMsg(HPacket cmd)
	{
		throw 0; // not implemented
	}

	HPacket Server::readPacket(int socket)
	{
		HPacket packet;

		const size_t buf_size = 1024;
		const size_t header_size = sizeof(uint32_t)*3;

		char data[buf_size];

		ssize_t bytes_read = recv(socket,data,header_size,0);
		if(bytes_read!=header_size)
			throw 99;
		memcpy(static_cast<void*>(&packet),data,header_size);

		packet.allocate();

		//read packet data
		while(bytes_read!=packet.data_size)
		{
			ssize_t round_read = recv(socket,
									packet.data+bytes_read,
									packet.data_size-bytes_read,
									0);
			if(round_read<=0)
			{
				std::cout<<"pipeline broken\n"<<"receive failed"<<std::endl;
				throw -1;
			}

			bytes_read += round_read;
		}

		return packet;
	}


	void *Server::clientThreadLoop(void *param)
	{
		Server *local_this = reinterpret_cast<Server*>(param);
		int accepted_socket = *reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(param)+sizeof(Server*));

		while(true)
		{
			HPacket packet = readPacket(accepted_socket);
			std::cout<<"Packet accepted"<<std::endl;
			local_this->processMsg(packet);
			packet.free();
		}

		shutdown(accepted_socket,2);

		return nullptr;
	}

	int Server::start()
	{
		struct sockaddr_in server_addr;

		listen_socket = socket(AF_INET,SOCK_STREAM,0);

		if(listen_socket==-1)
		{
			std::cout<<"Failed to create socket"<<std::endl;
			throw 6;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(1234);

		int status = bind(listen_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr));

		if(status==-1)
		{
			std::cout<<"Failed to bind address to socket"<<std::endl;
			throw 7;
		}

		pthread_t thread;

		pthread_create(&thread,nullptr,listenClients,static_cast<void*>(this));

		return 1;
	}

	void Server::stop()
	{
		done = true;
		pthread_join(listen_thread,nullptr);
	}


	Server::~Server(void)
	{
		stop();
	}
}