#include "app.h"
#include "usbd_cdc_if.h"
#include "ma782.h"
#include "string.h"

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;

static struct Ma782_Handle finger1base = {.hspi = &hspi1, .csPort = CS1_GPIO_Port, .csPin = CS1_Pin, .dir = 1, .zero = 24500};
static struct Ma782_Handle finger1middle = {.hspi = &hspi1, .csPort = CS3_GPIO_Port, .csPin = CS3_Pin, .dir = 0, .zero = 41000};
static struct Ma782_Handle finger1tip = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 47500};

//static struct Ma782_Handle finger2base = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger2middle = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger2tip = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//
//static struct Ma782_Handle finger3base = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger3middle = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger3tip = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//
//static struct Ma782_Handle finger4base = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger4middle = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger4tip = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//
//static struct Ma782_Handle finger5base = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger5middle = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};
//static struct Ma782_Handle finger5tip = {.hspi = &hspi1, .csPort = CS2_GPIO_Port, .csPin = CS2_Pin, .dir = 1, .zero = 0};

static struct Ma782_Handle* sensors[APP_SENSORS] = {
		&finger1base, &finger1middle, &finger1tip,
//		&finger2base, &finger2middle, &finger2tip,
//		&finger3base, &finger3middle, &finger3tip,
//		&finger4base, &finger4middle, &finger4tip,
//		&finger5base, &finger5middle, &finger5tip,
};

static volatile uint8_t flag = 0;

void App_Init(void) {
	for (uint8_t i = 0; i < APP_SENSORS; i++) {
		Ma782_Init(sensors[i]);
	}
	HAL_TIM_Base_Start_IT(&htim1);
}

void App_Update(void) {
	if (flag) {
		uint8_t data[APP_SENSORS*2];
		for (uint8_t i = 0; i < APP_SENSORS; i++) {
			Ma782_Update(sensors[i]);
			memcpy(data+(i*2), (uint8_t*)&sensors[i]->angle, 2);
		}

		CDC_Transmit_FS(data, APP_SENSORS*2);
		flag = 0;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	flag = 1;
}
