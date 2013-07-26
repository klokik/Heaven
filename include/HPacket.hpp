#ifndef HPACKET_H_
#define HPACKET_H_

namespace heaven
{
	struct HPacket
	{
		enum Msg {
			PING = 1,
			PAUSE,
			STOP,
			GET_ALL,
			ADD_ISLAND,
			UPD_ISLAND,
			REM_ISLAND,
			ADD_SHIP,
			UPD_SHIP,
			REM_SHIP,
			ADD_SIDE,
			UPD_SIDE,
			REM_SIDE,
			TRANSFER,
			DISCONNECT,
			TEXT
		};

		uint32_t msg_type = 0;
		uint32_t data_size = 0;
		char *data = nullptr;

		void allocate(void)
		{
			if(data_size)
				data = new char[data_size];
		}

		void free(void)
		{
			if(data)
				delete []data;
		}
	};
}

#endif /* HPACKET_H_ */