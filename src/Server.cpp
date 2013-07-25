#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <iostream>

#include "Server.hpp"

namespace heaven
{
	Server::Client::Client(int socket,Server *server)
	{
		this->socket = socket;
		this->server = server;
		assert(server);

		thread_owner = new Client*;
		*thread_owner = this;
		empty = false;
	}

	Server::Client::Client(Client &&c)
	{
		this->operator=(std::move(c));
	}

	Server::Client &Server::Client::operator=(Client &&c)
	{
		thread = c.thread;
		thread_owner = c.thread_owner;
		socket = c.socket;
		server = c.server;
		done = c.done;

		c.empty = true;
		empty = false;

		return *this;
	}

	int Server::Client::getUid(void)
	{
		static int s_uid = 1;
		std::cout<<"Warning: SHIT!!!"<<std::endl;
		return s_uid++;
	}

	void *Server::Client::listen(void *param)
	{
		Server::Client *local_this = reinterpret_cast<Server::Client*>(param);

		while(!local_this->done)
		{
			try
			{
				HPacket packet = local_this->server->readPacket(local_this->socket);
				std::cout<<"Packet accepted"<<std::endl;
				local_this->handlePacket(packet);
				packet.free();
			}
			catch(int i)
			{
				std::cout<<"read client failed: "<<i<<std::endl;
				break;
			}
		}

		local_this->disconnect();

		return nullptr;
	}

	void Server::Client::start(void)
	{
		pthread_create(&thread,nullptr,listen,static_cast<void*>(*thread_owner));
	}

	void Server::Client::send(HPacket cmd)
	{
		assert(socket!=-1);

		write(socket,reinterpret_cast<char*>(&cmd),sizeof(uint32_t)*2);
		write(socket,cmd.data,sizeof(char)*cmd.data_size);
	}

	int Server::Client::ping(void)
	{
		throw 0;
	}

	void Server::Client::disconnect(void)
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

	void Server::Client::kill(std::string msg)
	{
		//send text message
		disconnect();
	}

	void Server::Client::handlePacket(HPacket cmd)
	{
		server->processMsg(cmd);
	}

	Server::Client::~Client(void)
	{
		if(!empty)
		{
			disconnect();
			pthread_join(thread,nullptr);
			delete thread_owner;
		}
	}

	Server::Server(HeavenWorld *world)
	{

	}

	void *Server::listenClients(void *param)
	{
		Server *local_this = reinterpret_cast<Server*>(param);

		int status = listen(local_this->listen_socket, 5);

		if(status==-1)
		{
			std::cout<<"Failed to open port for listening"<<std::endl;
			throw 9;
		}

		std::cout<<"OK, listening client on port "<<local_this->port<<std::endl;

		local_this->done = false;
		while(!local_this->done)
		{
			local_this->acceptClient(local_this->listen_socket);
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

		Client client(client_socket,this);
		client.start();
		//clients[client.getUid()] = client;
		clients.push_back(std::move(client));
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

		ssize_t bytes_read = 0;
		while(bytes_read!=header_size)
		{
			recv(socket,data+bytes_read,header_size-bytes_read,0);
		}

		memcpy(static_cast<void*>(&packet),data,header_size);

		packet.allocate();

		//read packet data
		bytes_read = 0;
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

	int Server::start()
	{
		port = 1234;

		struct sockaddr_in server_addr;

		listen_socket = socket(AF_INET,SOCK_STREAM,0);

		if(listen_socket==-1)
		{
			std::cout<<"Failed to create socket"<<std::endl;
			throw 6;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(port);

		int status = bind(listen_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr));

		if(status==-1)
		{
			std::cout<<"Failed to bind address to socket"<<std::endl;
			throw 7;
		}

		pthread_create(&listen_thread,nullptr,listenClients,static_cast<void*>(this));

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