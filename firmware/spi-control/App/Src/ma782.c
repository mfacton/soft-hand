#include "ma782.h"
#include "usbd_cdc_if.h"

static void ma782_transmit_receive(struct Ma782_Handle* handle, uint8_t command, uint8_t data, uint8_t* resp) {
	uint8_t tx[2] = {command, data};
	uint8_t temp[2];
	command = command>>8 | command<<8;
	HAL_SPI_TransmitReceive(handle->hspi, tx, temp, 2, HAL_MAX_DELAY);
	resp[0] = temp[1];
	resp[1] = temp[0];
}

static void ma782_send_frame(struct Ma782_Handle* handle, uint8_t command, uint8_t data, uint8_t* resp) {
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 0);
	ma782_transmit_receive(handle, command, data, (uint8_t*)&handle->angle);
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 0);
	ma782_transmit_receive(handle, 0, 0, resp);
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 1);
}

//static uint8_t ma782_read_register(struct Ma782_Handle* handle, uint8_t reg) {
//	uint8_t resp[2];
//	ma782_send_frame(handle, 0b010 << 5 | reg, 0, resp);
//	return resp[0];
//}

static void ma782_write_register(struct Ma782_Handle* handle, uint8_t reg, uint8_t data) {
	uint8_t resp[2];
	ma782_send_frame(handle, 0b100 << 5 | reg, data, resp);
	if (resp[0] != data) {
		Error_Handler();
	}
}

void Ma782_Init(struct Ma782_Handle* handle) {
	handle->angle = 0;

	ma782_write_register(handle, MA782_REG_DIRECTION, handle->dir << 7);
	ma782_write_register(handle, MA782_REG_ZERO_L, handle->zero & 0xff);
	ma782_write_register(handle, MA782_REG_ZERO_H, handle->zero >> 8);

	Ma782_ClearError(handle);
}

void Ma782_Update(struct Ma782_Handle* handle) {
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 0);
	ma782_transmit_receive(handle, 0, 0, (uint8_t*)&handle->angle);
	HAL_GPIO_WritePin(handle->csPort, handle->csPin, 1);
}

void Ma782_ClearError(struct Ma782_Handle* handle) {
	ma782_send_frame(handle, 0b001 << 5, 0, (uint8_t*)&handle->angle);
}
