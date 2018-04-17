#ifndef __USER_RF433_H
#define __USER_RF433_H

#include "driver/rf1527.h"


typedef struct
{
	u8 Index;
	struct
	{
		u8 data[MAX_BYTES];
		u8 bits;
		u8 nums;
	}
}RF_DATA_BUF;









#endif


