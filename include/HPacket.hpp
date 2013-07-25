#ifndef HPACKET_H_
#define HPACKET_H_

namespace heaven
{
	struct HPacket
	{
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