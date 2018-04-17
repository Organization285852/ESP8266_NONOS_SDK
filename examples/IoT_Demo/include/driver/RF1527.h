#ifndef __RF1527_H
#define __RF1527_H

#include "eagle_soc.h"
#include "Gpio.h"

#define RFOUT_IO_MUX     PERIPHS_IO_MUX_GPIO4_U
#define RFOUT_IO_NUM     4
#define RFOUT_IO_FUNC    FUNC_GPIO4

#define RFIN_IO_MUX     PERIPHS_IO_MUX_SD_DATA2_U
#define RFIN_IO_NUM     9
#define RFIN_IO_FUNC    FUNC_GPIO9


#define FROUT(on)     GPIO_OUTPUT_SET(RFOUT_IO_NUM, on)
#define RFIN()			   GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_IO_NUM))

void rf1527_init(void);


#endif




