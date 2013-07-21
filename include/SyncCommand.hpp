#ifndef SYNCCOMMAND_H_
#define SYNCCOMMAND_H_

namespace heaven
{
	typedef struct
	{
		uint32_t side_uid;
		uint32_t action;
		uint32_t data1_size;
		void *data1;
		uint32_t data2_size;
		void *data2;
	} SyncCommand;
}

#endif /* SYNCCOMMAND_H_ */