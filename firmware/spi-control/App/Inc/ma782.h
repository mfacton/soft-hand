#ifndef INC_MA782_H_
#define INC_MA782_H_

#include "main.h"

#define MA782_REG_ZERO_L 0x00
#define MA782_REG_ZERO_H 0x01

#define MA782_REG_TRIM_BIAS 0x02

#define MA782_REG_TRIM_XY 0x03

#define MA782_REG_CYCLE_TIME_L 0x04
#define MA782_REG_CYCLE_TIME_H 0x05

#define MA782_REG_MG_CONFIG 0x06

#define MA782_REG_NEW_DATA_CONFIG 0x07
/////
#define MA782_REG_THR 0x08

#define MA782_REG_DIRECTION 0x09

#define MA782_REG_REF 0x0A

#define MA782_REG_ASC 0x0B

#define MA782_REG_FW 0x0E

#define MA782_REG_ERR 0x1A

#define MA782_REG_MG 0x1B

struct Ma782_Handle {
	// configuration
	SPI_HandleTypeDef *hspi;

	GPIO_TypeDef *csPort;
	uint16_t csPin;

	uint8_t dir;
	uint16_t zero;

	// internal
	uint16_t angle;
};

void Ma782_Init(struct Ma782_Handle* handle);
void Ma782_Update(struct Ma782_Handle* handle);

void Ma782_ClearError(struct Ma782_Handle* handle);

#endif
